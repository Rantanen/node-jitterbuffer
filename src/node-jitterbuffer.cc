
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
			NanScope();

			REQ_OBJ_ARG( 0, packet );

			Local<Object> data = Local<Object>::Cast(
					packet->Get( NanNew<String>( "data" ) ) );
			int timestamp = packet->Get( NanNew<String>( "timestamp" ) )->Int32Value();
			int span = packet->Get( NanNew<String>( "span" ) )->Int32Value();
			int sequence = packet->Get( NanNew<String>( "sequence" ) )->Int32Value();

			// Userdata is optional.
			int userData = 0;
			Handle<Value> userDataHandle = packet->Get( NanNew<String>( "userData" ) );
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
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );
			jitter_buffer_put( self->jitterBuffer, &speexPacket );

			NanReturnUndefined();
		}

		static NAN_METHOD(Get) {
			NanScope();

			REQ_INT_ARG( 0, desiredSpan );

			char data[4096];
			JitterBufferPacket jitterPacket;
			jitterPacket.data = data;
			jitterPacket.len = 4096;
			spx_int32_t start_offset = 0;

			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );
			int returnValue = jitter_buffer_get( self->jitterBuffer, &jitterPacket, desiredSpan, &start_offset );
			if( returnValue == JITTER_BUFFER_OK ) {

				CREATE_BUFFER( data, jitterPacket.data, jitterPacket.len );

				Local<Object> packet = NanNew<Object>();
				packet->Set( NanNew<String>( "data" ), data );
				packet->Set( NanNew<String>( "timestamp" ), NanNew<Number>( jitterPacket.timestamp ) );
				packet->Set( NanNew<String>( "span" ), NanNew<Number>( jitterPacket.span ) );
				packet->Set( NanNew<String>( "sequence" ), NanNew<Number>( jitterPacket.sequence ) );
				packet->Set( NanNew<String>( "userData" ), NanNew<Number>( jitterPacket.user_data ) );

				NanReturnValue( packet );
			} else {
				NanReturnValue( NanNew<Number>( returnValue ) );
			}
		}

		static NAN_METHOD(Tick) {
			NanScope();
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );
			jitter_buffer_tick( self->jitterBuffer );

			NanReturnUndefined();
		}

		static NAN_METHOD(GetMargin) {
			NanScope();
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );

			int margin;
			jitter_buffer_ctl( self->jitterBuffer, JITTER_BUFFER_GET_MARGIN, &margin );

			NanReturnValue( NanNew<Number>( margin ) );
		}

		static NAN_METHOD(SetMargin) {
			NanScope();

			REQ_INT_ARG( 0, margin );

			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );
			jitter_buffer_ctl( self->jitterBuffer, JITTER_BUFFER_SET_MARGIN, &margin );

			NanReturnValue( NanNew<Number>( margin ) );
		}

		static NAN_METHOD(New) {
			NanScope();

			if( !args.IsConstructCall()) {
				NanThrowTypeError("Use the new operator to construct the NodeJitterBuffer.");
				NanReturnUndefined();
			}

			OPT_INT_ARG(0, stepSize, 10);

			NodeJitterBuffer* encoder = new NodeJitterBuffer( stepSize );

			encoder->Wrap( args.This() );
			NanReturnValue( args.This() );
		}

		static void Init(Handle<Object> exports) {
			Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
			tpl->SetClassName(NanNew<String>("JitterBuffer"));
			tpl->InstanceTemplate()->SetInternalFieldCount(1);

			tpl->PrototypeTemplate()->Set( NanNew<String>("put"),
				NanNew<FunctionTemplate>( Put )->GetFunction() );

			tpl->PrototypeTemplate()->Set( NanNew<String>("get"),
				NanNew<FunctionTemplate>( Get )->GetFunction() );

			tpl->PrototypeTemplate()->Set( NanNew<String>("tick"),
				NanNew<FunctionTemplate>( Tick )->GetFunction() );

			tpl->PrototypeTemplate()->Set( NanNew<String>("setMargin"),
				NanNew<FunctionTemplate>( SetMargin )->GetFunction() );

			tpl->PrototypeTemplate()->Set( NanNew<String>("getMargin"),
				NanNew<FunctionTemplate>( GetMargin )->GetFunction() );

			exports->Set(NanNew<String>("JitterBuffer"), tpl->GetFunction());
		}
};


void NodeInit(Handle<Object> exports) {
	NodeJitterBuffer::Init( exports );
}

NODE_MODULE(node_jitterbuffer, NodeInit)
