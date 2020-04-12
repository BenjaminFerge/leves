grpcurl -d '{"type":"alma"}' -plaintext -import-path ../protos -proto yess.proto localhost:2929 yess.YessService/CreateStream
