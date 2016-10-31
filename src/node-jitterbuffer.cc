
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>
#include "speex/speex_jitter.h"
#include "common.h"
#include <nan.h>

#include <string.h>

using namespace node;
using namespace v8;

class NodeJitterBuffer : public ObjectWrap {
	private:
		JitterBuffer* jitterBuffer;

	protected:

	public:
	   	NodeJitterBuffer( int step_size ) {
			jitterBuffer = jitter_buffer_init( step_size );
		}

		~NodeJitterBuffer() {
			fprintf( stderr, "\nDestroyer\n\n" );
			if( jitterBuffer != NULL ) {
				jitter_buffer_destroy( jitterBuffer );
				jitterBuffer = NULL;
			}
		}

		static NAN_METHOD(Put) {

			REQ_OBJ_ARG( 0, packet );

			Local<Object> data = Local<Object>::Cast(
					packet->Get( Nan::New( "data" ).ToLocalChecked() ) );
			int timestamp = packet->Get( Nan::New( "timestamp" ).ToLocalChecked() )->Int32Value();
			int span = packet->Get( Nan::New( "span" ).ToLocalChecked() )->Int32Value();
			int sequence = packet->Get( Nan::New( "sequence" ).ToLocalChecked() )->Int32Value();

			// Userdata is optional.
			int userData = 0;
			Handle<Value> userDataHandle = packet->Get( Nan::New( "userData" ).ToLocalChecked() );
			if( !userDataHandle.IsEmpty() ) {
				userData = userDataHandle->Int32Value();
			}

			JitterBufferPacket speexPacket;
			speexPacket.data = Buffer::Data( data );
			speexPacket.len = Buffer::Length( data );
			speexPacket.timestamp = timestamp;
			speexPacket.span = span;
			speexPacket.sequence = sequence;
			speexPacket.user_data = userData;

			// Put copies the data so it's okay to pass a reference to a local variable.
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( info.This() );
			jitter_buffer_put( self->jitterBuffer, &speexPacket );
		}

		static NAN_METHOD(Get) {

			REQ_INT_ARG( 0, desiredSpan );

			char data[4096];
			JitterBufferPacket jitterPacket;
			jitterPacket.data = data;
			jitterPacket.len = 4096;
			spx_int32_t start_offset = 0;

			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( info.This() );
			int returnValue = jitter_buffer_get( self->jitterBuffer, &jitterPacket, desiredSpan, &start_offset );
			if( returnValue == JITTER_BUFFER_OK ) {

				CREATE_BUFFER( data, jitterPacket.data, jitterPacket.len );

				Local<Object> packet = Nan::New<Object>();
				packet->Set( Nan::New( "data" ).ToLocalChecked(), data );
				packet->Set( Nan::New( "timestamp" ).ToLocalChecked(), Nan::New<Number>( jitterPacket.timestamp ) );
				packet->Set( Nan::New( "span" ).ToLocalChecked(), Nan::New<Number>( jitterPacket.span ) );
				packet->Set( Nan::New( "sequence" ).ToLocalChecked(), Nan::New<Number>( jitterPacket.sequence ) );
				packet->Set( Nan::New( "userData" ).ToLocalChecked(), Nan::New<Number>( jitterPacket.user_data ) );

				info.GetReturnValue().Set( packet );
			} else {
				info.GetReturnValue().Set( Nan::New<Number>( returnValue ) );
			}
		}

		static NAN_METHOD(Tick) {

			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( info.This() );
			jitter_buffer_tick( self->jitterBuffer );

		}

		static NAN_METHOD(GetMargin) {
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( info.This() );

			int margin;
			jitter_buffer_ctl( self->jitterBuffer, JITTER_BUFFER_GET_MARGIN, &margin );

			info.GetReturnValue().Set( Nan::New<Number>( margin ) );
		}

		static NAN_METHOD(SetMargin) {

			REQ_INT_ARG( 0, margin );

			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( info.This() );
			jitter_buffer_ctl( self->jitterBuffer, JITTER_BUFFER_SET_MARGIN, &margin );

			info.GetReturnValue().Set( Nan::New<Number>( margin ) );
		}

		static NAN_METHOD(New) {

			if( !info.IsConstructCall()) {
				return Nan::ThrowTypeError("Use the new operator to construct the NodeJitterBuffer.");
			}

			OPT_INT_ARG(0, stepSize, 10);

			NodeJitterBuffer* encoder = new NodeJitterBuffer( stepSize );

			encoder->Wrap( info.This() );
			info.GetReturnValue().Set( info.This() );
		}

		static void Init(Handle<Object> exports) {
			Nan::HandleScope scope;

			Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
			tpl->SetClassName(Nan::New("JitterBuffer").ToLocalChecked());
			tpl->InstanceTemplate()->SetInternalFieldCount(1);

			Nan::SetPrototypeMethod( tpl, "put", Put );
			Nan::SetPrototypeMethod( tpl, "get", Get );
			Nan::SetPrototypeMethod( tpl, "tick", Tick );
			Nan::SetPrototypeMethod( tpl, "setMargin", SetMargin );
			Nan::SetPrototypeMethod( tpl, "getMargin", GetMargin );

			exports->Set(
				Nan::New("JitterBuffer").ToLocalChecked(), 
				tpl->GetFunction());
		}
};


void NodeInit(Handle<Object> exports) {
	NodeJitterBuffer::Init( exports );
}

NODE_MODULE(node_jitterbuffer, NodeInit)
