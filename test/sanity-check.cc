import dxx.selftest;

static const dxx::selftest::unit_test sanity_check{
    "/selftest/sanity-check", [] {
        dxx::selftest::test(true);
    }
};
