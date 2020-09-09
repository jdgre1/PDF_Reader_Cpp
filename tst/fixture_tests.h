#pragma once
#ifndef FIXTURE_TESTS__H__
#define FIXTURE_TESTS__H__

#pragma once

#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>

//class CalibParams_Mock

class SetupTest : public testing::Test {

public:
	//CalibParams m_cb;

	// Constructor / Destructor
	SetupTest();
	~SetupTest();
};
#endif


/*
#include <bank_account.h>

class BankAccountTest : public testing::Test {

public:
	BankAccount* m_account;

	BankAccountTest() : m_account(new BankAccount) {};

	~BankAccountTest() {
		delete m_account;
	}

	struct account_state {
		int initial_balance;
		int withdraw_amount;
		int final_balance;
		bool success;
	};

};

class ParamTest_withdraw : public BankAccountTest, public testing::WithParamInterface<BankAccountTest::account_state> {

public:
	ParamTest_withdraw() {
		m_account->m_balance = GetParam().initial_balance;

	}

};
*/
