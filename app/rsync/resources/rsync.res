
namespace rsync_service;

message EmptyParams {
}


message FileQuery {
  optional string path = 1;
}
message FileQueryResponse {
  optional bool file_found = 1;
  optional uint32 file_size = 2;
}

message JobStart {
  optional string path = 1;
}
message JobStartResponse {
  optional bool file_found = 1;
  optional uint32 file_size = 2;
  optional int32 job_id = 3;
}

resource RsyncService {
  allow_exceptions = true;
  FileQueryResponse queryFile(FileQuery);
  JobStartResponse pull(JobStart);
  JobStartResponse push(JobStart);
}
