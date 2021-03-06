//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.

#include "db/event_logger_helpers.h"

namespace rocksdb {

namespace {
inline double SafeDivide(double a, double b) { return b == 0.0 ? 0 : a / b; }
}  // namespace

void EventLoggerHelpers::AppendCurrentTime(JSONWriter* jwriter) {
  *jwriter << "time_micros"
           << std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::system_clock::now().time_since_epoch()).count();
}

void EventLoggerHelpers::LogTableFileCreation(
    EventLogger* event_logger, int job_id, uint64_t file_number,
    uint64_t file_size, const TableProperties& table_properties) {
  JSONWriter jwriter;
  AppendCurrentTime(&jwriter);
  jwriter << "job" << job_id
           << "event" << "table_file_creation"
           << "file_number" << file_number
           << "file_size" << file_size;

  // table_properties
  {
    jwriter << "table_properties";
    jwriter.StartObject();

    // basic properties:
    jwriter << "data_size" << table_properties.data_size
            << "index_size" << table_properties.index_size
            << "filter_size" << table_properties.filter_size
            << "raw_key_size" << table_properties.raw_key_size
            << "raw_average_key_size" << SafeDivide(
                table_properties.raw_key_size,
                table_properties.num_entries)
            << "raw_value_size" << table_properties.raw_value_size
            << "raw_average_value_size" << SafeDivide(
               table_properties.raw_value_size, table_properties.num_entries)
            << "num_data_blocks" << table_properties.num_data_blocks
            << "num_entries" << table_properties.num_entries
            << "filter_policy_name" << table_properties.filter_policy_name;

    // user collected properties
    for (const auto& prop : table_properties.user_collected_properties) {
      jwriter << prop.first << prop.second;
    }
    jwriter.EndObject();
  }

  event_logger->Log(jwriter);
}

}  // namespace rocksdb
