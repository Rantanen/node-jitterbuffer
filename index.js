
var jitterbuffer = require('bindings')('node-jitterbuffer');

exports.JitterBuffer = jitterbuffer.JitterBuffer;

// Following error codes are for get()
// They are taken directly from NodeJS

/** Packet is lost or is late */
exports.MISSING = 1;

/** A "fake" packet is meant to be inserted here to increase buffering */
exports.INSERTION = 2;

/** There was an error in the jitter buffer */
exports.INTERNAL_ERROR = -1;

/** Invalid argument */
exports.BAD_ARGUMENT = -2;

