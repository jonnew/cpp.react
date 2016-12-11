
//          Copyright Sebastian Jeckel 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "TransactionTest.h"
#include "TestUtil.h"

#include "react/engine/PulsecountEngine.h"
#include "react/engine/ToposortEngine.h"
#include "react/engine/SubtreeEngine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace {

using namespace react;

using P1 = DomainParams<sequential_concurrent,ToposortEngine>;
using P2 = DomainParams<parallel_concurrent,ToposortEngine>;
using P3 = DomainParams<parallel_concurrent,PulsecountEngine>;
using P4 = DomainParams<parallel_concurrent,SubtreeEngine>;

INSTANTIATE_TYPED_TEST_CASE_P(SeqToposort, TransactionTest, P1);
INSTANTIATE_TYPED_TEST_CASE_P(ParToposort, TransactionTest, P2);
INSTANTIATE_TYPED_TEST_CASE_P(Pulsecount, TransactionTest, P3);
INSTANTIATE_TYPED_TEST_CASE_P(Subtree, TransactionTest, P4);

} // ~namespace
