function Decoder(bytes, port, uplink_info) {
    /*
      The uplink_info variable is an OPTIONAL third parameter that provides the following:
    
      uplink_info = {
        type: "join",
        uuid: <UUIDv4>,
        id: <device id>,
        name: <device name>,
        dev_eui: <dev_eui>,
        app_eui: <app_eui>,
        metadata: {...},
        fcnt: <integer>,
        reported_at: <timestamp>,
        port: <integer>,
        devaddr: <devaddr>,
        hotspots: {...},
        hold_time: <integer>
      }
    */

    var decoded = {};

    function to_int(bytes) {
        value = bytes[0] * 256 + bytes[1];
        if (value >= 32768) {
            value = 32768 - value;
        }
        return value;
    }

    function to_float(bytes) {
        value = to_int(bytes) / 100.0;
        return value;
    }

    if (uplink_info && port == 8) {
        decoded.temperature = to_float(bytes.slice(0, 2));
        decoded.humidity = to_float(bytes.slice(2, 4));
        decoded.uv = to_int(bytes.slice(4, 6));
        decoded.index = to_int(bytes.slice(6, 8));
    }

    return decoded;
}
