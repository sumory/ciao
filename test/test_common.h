#pragma once
#include <gtest/gtest.h>
#include "ciao/define.h"

class CiaoTest : public testing::Test {
 public:
    virtual void SetUp() {}
    virtual void TearDown() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

class TrieTest : public CiaoTest {
 public:
    virtual void SetUp() {}
    virtual void TearDown() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

class UtilsTest : public CiaoTest {
 public:
    virtual void SetUp() {}
    virtual void TearDown() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};

class AppTest : public CiaoTest {
 public:
    virtual void SetUp() {}
    virtual void TearDown() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
};
