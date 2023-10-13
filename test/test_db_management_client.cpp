/**
 * Copyright 2022 AntGroup CO., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#include <iostream>
#include <fstream>
#include <filesystem>

#include "./ut_utils.h"
#include "./test_tools.h"
#include "fma-common/utils.h"
#include "gtest/gtest.h"
#include "server/db_management_client.h"

class TestDBManagementClient : public TuGraphTest {};

TEST_F(TestDBManagementClient, DBManagementClient) {
    using namespace lgraph;

    // set up cmd
    std::string db_management_folder = "../../deps/tugraph-db-management/";
    std::string sqlite_db_file = "tugraph_db_management.db";
    std::string temp_folder = db_management_folder + "/ut_temp";

    // set up sqlite db file
    std::string cmd;
    int rt;
    // cmd = "touch ../../deps/tugraph-db-management/tugraph_db_management_ut_temp.db";
    cmd = "mkdir " + temp_folder;
    rt = system(cmd.c_str());
    UT_EXPECT_EQ(rt, 0);
    cmd = "mv " + db_management_folder + sqlite_db_file + " " + temp_folder + sqlite_db_file;
    rt = system(cmd.c_str());
    UT_EXPECT_EQ(rt, 0);
    cmd = "touch " + db_management_folder + sqlite_db_file;
    rt = system(cmd.c_str());
    UT_EXPECT_EQ(rt, 0);

    // set up test veriables
    std::string exception_msg = "failed to connect to db management.";
    std::string host = "127.0.0.1";
    std::string port = "8888";
    std::int64_t start_time = 169413845845;
    std::string period = "IMMEDIATE";
    std::string name = "Khop_test";
    std::string type = "Python";
    std::string user = "user";
    std::int64_t create_time = 1694138458457;
    int job_id = 1;
    std::string status = "SUCCESS";
    std::int64_t runtime = 100;
    std::string result = "this is only a test of result";

    // test exception handle
    try {
        DBManagementClient::GetInstance()
            .CreateJob(host, port, start_time, period, name, type, user, create_time);
        UT_EXPECT_EQ(1, 0);
    } catch(std::exception& e) {
        UT_EXPECT_EQ(e.what(), exception_msg);
    }
    try {
        DBManagementClient::GetInstance().UpdateJob(host, port, job_id, status, runtime, result);
        UT_EXPECT_EQ(1, 0);
    } catch(std::exception& e) {
        UT_EXPECT_EQ(e.what(), exception_msg);
    }
    try {
        DBManagementClient::GetInstance().ReadJob(host, port);
        UT_EXPECT_EQ(1, 0);
    } catch(std::exception& e) {
        UT_EXPECT_EQ(e.what(), exception_msg);
    }
    try {
        DBManagementClient::GetInstance().ReadJobResult(host, port, job_id);
        UT_EXPECT_EQ(1, 0);
    } catch(std::exception& e) {
        UT_EXPECT_EQ(e.what(), exception_msg);
    }
    try {
        DBManagementClient::GetInstance().DeleteJob(host, port, job_id);
        UT_EXPECT_EQ(1, 0);
    } catch(std::exception& e) {
        UT_EXPECT_EQ(e.what(), exception_msg);
    }

    // start db management
    cmd = "cd " + db_management_folder + " && " + "bash ut_start.sh";
    rt = system(cmd.c_str());
    UT_EXPECT_EQ(rt, 0);
    // sleep to wait db management start
    fma_common::SleepS(300);

    // test crud
    try {
        job_id = DBManagementClient::GetInstance()
            .CreateJob(host, port, start_time, period, name, type, user, create_time);
        UT_EXPECT_EQ(1, job_id);
    } catch(std::exception& e) {
        DEBUG_LOG(ERROR) << e.what();
        UT_EXPECT_EQ(1, 0);
    }
    try {
        DBManagementClient::GetInstance().UpdateJob(host, port, job_id, status, runtime, result);
    } catch(std::exception& e) {
        DEBUG_LOG(ERROR) << e.what();
        UT_EXPECT_EQ(1, 0);
    }
    try {
        std::vector<com::antgroup::tugraph::Job> jobs =
            DBManagementClient::GetInstance().ReadJob(host, port);
        UT_EXPECT_EQ(1, jobs.size());
    } catch(std::exception& e) {
        DEBUG_LOG(ERROR) << e.what();
        UT_EXPECT_EQ(1, 0);
    }
    try {
        com::antgroup::tugraph::JobResult job_result =
            DBManagementClient::GetInstance().ReadJobResult(host, port, job_id);
        UT_EXPECT_EQ(result, job_result.result());
    } catch(std::exception& e) {
        DEBUG_LOG(ERROR) << e.what();
        UT_EXPECT_EQ(1, 0);
    }
    try {
        DBManagementClient::GetInstance().DeleteJob(host, port, job_id);
    } catch(std::exception& e) {
        DEBUG_LOG(ERROR) << e.what();
        UT_EXPECT_EQ(1, 0);
    }
    try {
        std::vector<com::antgroup::tugraph::Job> jobs =
            DBManagementClient::GetInstance().ReadJob(host, port);
        UT_EXPECT_EQ(0, jobs.size());
    } catch(std::exception& e) {
        DEBUG_LOG(ERROR) << e.what();
        UT_EXPECT_EQ(1, 0);
    }

    // stop db management
    cmd = "cd " + db_management_folder + " && " + "bash ut_stop.sh ";
    rt = system(cmd.c_str());
    UT_EXPECT_EQ(rt, 0);

    // reset sqlite db file
    cmd = "rm " + db_management_folder + sqlite_db_file;
    rt = system(cmd.c_str());
    UT_EXPECT_EQ(rt, 0);
    cmd = "mv " + temp_folder + sqlite_db_file + " " + db_management_folder + sqlite_db_file;
    rt = system(cmd.c_str());
    UT_EXPECT_EQ(rt, 0);
    cmd = "rm -rf " + temp_folder;
    rt = system(cmd.c_str());
}