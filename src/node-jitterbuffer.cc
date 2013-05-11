
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include "speex/speex_jitter.h"
#include "common.h"

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


		static Handle<Value> Put( const Arguments& args ) {
			HandleScope scope;

			REQ_OBJ_ARG( 0, packet );

			Local<Object> data = Local<Object>::Cast(
					packet->Get( String::New( "data" ) ) );
			int timestamp = packet->Get( String::New( "timestamp" ) )->Int32Value();
			int span = packet->Get( String::New( "span" ) )->Int32Value();
			int sequence = packet->Get( String::New( "sequence" ) )->Int32Value();

			JitterBufferPacket speexPacket;
			speexPacket.data = Buffer::Data( data );
			speexPacket.len = Buffer::Length( data );
			speexPacket.timestamp = timestamp;
			speexPacket.span = span;
			speexPacket.sequence = sequence;

			// Put copies the data so it's okay to pass a reference to a local variable.
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );
			jitter_buffer_put( self->jitterBuffer, &speexPacket );

			return scope.Close( Undefined() );
		}

		static Handle<Value> Get( const Arguments& args ) {
			HandleScope scope;

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

				Local<Object> packet = Object::New();
				packet->Set( String::NewSymbol( "data" ), data );
				packet->Set( String::NewSymbol( "timestamp" ), Number::New( jitterPacket.timestamp ) );
				packet->Set( String::NewSymbol( "span" ), Number::New( jitterPacket.span ) );
				packet->Set( String::NewSymbol( "sequence" ), Number::New( jitterPacket.sequence ) );

				return scope.Close( packet );
			} else {
				return scope.Close( Number::New( returnValue ) );
			}
		}


		static Handle<Value> Tick( const Arguments& args ) {
			HandleScope scope;
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );
			jitter_buffer_tick( self->jitterBuffer );

			return scope.Close( Undefined() );
		}

		static Handle<Value> GetMargin( const Arguments& args ) {
			HandleScope scope;
			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );

			int margin;
			jitter_buffer_ctl( self->jitterBuffer, JITTER_BUFFER_GET_MARGIN, &margin );

			return scope.Close( Number::New( margin ) );
		}

		static Handle<Value> SetMargin( const Arguments& args ) {
			HandleScope scope;

			REQ_INT_ARG( 0, margin );

			NodeJitterBuffer* self = ObjectWrap::Unwrap<NodeJitterBuffer>( args.This() );
			jitter_buffer_ctl( self->jitterBuffer, JITTER_BUFFER_SET_MARGIN, &margin );

			return scope.Close( Number::New( margin ) );
		}

		static Handle<Value> New(const Arguments& args) {
			HandleScope scope;

			if( !args.IsConstructCall()) {
				return ThrowException(Exception::TypeError(
							String::New("Use the new operator to construct the NodeJitterBuffer.")));
			}

			OPT_INT_ARG(0, stepSize, 10);

			NodeJitterBuffer* encoder = new NodeJitterBuffer( stepSize );

			encoder->Wrap( args.This() );
			return scope.Close( args.This() );
		}

		static void Init(Handle<Object> exports) {
			Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
			tpl->SetClassName(String::NewSymbol("JitterBuffer"));
			tpl->InstanceTemplate()->SetInternalFieldCount(1);

			tpl->PrototypeTemplate()->Set( String::NewSymbol("put"),
					FunctionTemplate::New( Put )->GetFunction() );

			tpl->PrototypeTemplate()->Set( String::NewSymbol("get"),
					FunctionTemplate::New( Get )->GetFunction() );

			tpl->PrototypeTemplate()->Set( String::NewSymbol("tick"),
					FunctionTemplate::New( Tick )->GetFunction() );

			tpl->PrototypeTemplate()->Set( String::NewSymbol("setMargin"),
					FunctionTemplate::New( SetMargin )->GetFunction() );

			tpl->PrototypeTemplate()->Set( String::NewSymbol("getMargin"),
					FunctionTemplate::New( GetMargin )->GetFunction() );

			Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
			exports->Set(String::NewSymbol("JitterBuffer"), constructor);
		}
};


void init(Handle<Object> exports) {
	NodeJitterBuffer::Init( exports );
}

NODE_MODULE(node_jitterbuffer, init)

