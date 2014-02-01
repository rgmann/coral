
namespace eterm;

message EmptyParams {
}


message LedState {
  optional bool led_on = 1;
  optional int32 led_id = 2;
}

message FingerprintStatus {
  optional bool finger_pressed = 1;
  optional int32 num_records = 2;
}

// Overall status of Heimdall
message Status {
  optional bool success = 1;
  optional bool is_connected = 2;
  optional FingerprintStatus fp_status = 3;
}

resource HeimdallController {
  allow_exceptions = true;
  Status setLedState(LedState);
  Status getFingerprintStatus(EmptyParams);
}

