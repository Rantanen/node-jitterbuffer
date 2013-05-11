var JitterBuffer = require('../').JitterBuffer;

var jb = new JitterBuffer( 50 );
jb.setMargin( 30 );

var createByteBuffer = function( byte ) {
    var b = new Buffer(100);
    b[0] = byte;
    return b;
}

var timestamp = 0;
var addPacket = function() {
	timestamp += 10;

    jb.put({
        data: createByteBuffer(timestamp),
        span: 10,
        timestamp: timestamp
    });

    console.log( "Add: " + timestamp );
};

var getPacket = function() {
    var packet = jb.get( 10 );

    if( packet instanceof Object ) {
        console.log( "Get: " + packet.data[0] );
    } else {
        console.log( "Error: " + packet );
    }

	jb.tick();
};


addPacket();
getPacket();
addPacket();
addPacket();
addPacket();
getPacket();
addPacket();
getPacket();
getPacket();
addPacket();
getPacket();
getPacket();
addPacket();
getPacket();
getPacket();
addPacket();
getPacket();
addPacket();
getPacket();
addPacket();
getPacket();
addPacket();
getPacket();
getPacket();
addPacket();
getPacket();
getPacket();
addPacket();
getPacket();
getPacket();
addPacket();
getPacket();
addPacket();
getPacket();
getPacket();
addPacket();
getPacket();
addPacket();
getPacket();
getPacket();
getPacket();
getPacket();
getPacket();
getPacket();
