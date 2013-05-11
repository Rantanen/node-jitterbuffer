
var JitterBuffer = require('../').JitterBuffer;

var jb = new JitterBuffer( 10 );

var createByteBuffer = function( byte ) {
    var b = new Buffer(100);
    b[0] = byte;
    return b;
}

var addPacket = function( timestamp ) {

    jb.put({
        data: createByteBuffer(timestamp),
        span: 10,
        timestamp: timestamp
    });
};

addPacket(10);
addPacket(20);
// Missing packet 20
addPacket(40);
addPacket(60); // Packet 50 received before 40
addPacket(50);

for( var i = 0; i < 9; i++ ) {
    var packet = jb.get( 10 );

    if( packet instanceof Object ) {
        console.log( "Packet: " + packet.data[0] );
    } else {
        console.log( "Error: " + packet );
    }
    jb.tick();
}

