#ifndef test_intraprocess_config_h
#define test_intraprocess_config_h

#define BOOST_ALL_DYN_LINK 1
#define GTEST_LANG_CXX11 1

#ifdef _WIN32
#define ROOT_FILESYSTEM "C:/"
#else
#define ROOT_FILESYSTEM "/"
#endif

#endif // test_intraprocess_config_h
