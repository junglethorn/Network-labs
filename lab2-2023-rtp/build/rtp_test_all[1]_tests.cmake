add_test( RTP.NORMAL /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.NORMAL]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.NORMAL PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.NORMAL_SMALL_WINDOW /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.NORMAL_SMALL_WINDOW]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.NORMAL_SMALL_WINDOW PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.NORMAL_OPT /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.NORMAL_OPT]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.NORMAL_OPT PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.NORMAL_OPT_SMALL_WINDOW /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.NORMAL_OPT_SMALL_WINDOW]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.NORMAL_OPT_SMALL_WINDOW PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.RECEIVER_SINGLE_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.RECEIVER_SINGLE_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.RECEIVER_SINGLE_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.RECEIVER_SINGLE_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.RECEIVER_SINGLE_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.RECEIVER_SINGLE_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.RECEIVER_MIXED_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.RECEIVER_MIXED_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.RECEIVER_MIXED_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.RECEIVER_MIXED_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.RECEIVER_MIXED_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.RECEIVER_MIXED_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.SENDER_SINGLE_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.SENDER_SINGLE_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.SENDER_SINGLE_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.SENDER_SINGLE_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.SENDER_SINGLE_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.SENDER_SINGLE_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.SENDER_MIXED_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.SENDER_MIXED_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.SENDER_MIXED_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.SENDER_MIXED_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.SENDER_MIXED_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.SENDER_MIXED_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_RECEIVER_SINGLE_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_RECEIVER_SINGLE_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_RECEIVER_SINGLE_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_RECEIVER_SINGLE_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_RECEIVER_SINGLE_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_RECEIVER_SINGLE_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_RECEIVER_MIXED_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_RECEIVER_MIXED_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_RECEIVER_MIXED_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_RECEIVER_MIXED_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_RECEIVER_MIXED_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_RECEIVER_MIXED_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_SENDER_SINGLE_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_SENDER_SINGLE_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_SENDER_SINGLE_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_SENDER_SINGLE_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_SENDER_SINGLE_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_SENDER_SINGLE_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_SENDER_MIXED_1 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_SENDER_MIXED_1]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_SENDER_MIXED_1 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_SENDER_MIXED_2 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_SENDER_MIXED_2]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_SENDER_MIXED_2 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.NORMAL_HUGE_WINDOW /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.NORMAL_HUGE_WINDOW]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.NORMAL_HUGE_WINDOW PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.NORMAL_OPT_HUGE_WINDOW /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.NORMAL_OPT_HUGE_WINDOW]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.NORMAL_OPT_HUGE_WINDOW PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_RECEIVER_MIXED_3 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_RECEIVER_MIXED_3]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_RECEIVER_MIXED_3 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_RECEIVER_MIXED_4 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_RECEIVER_MIXED_4]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_RECEIVER_MIXED_4 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_SENDER_MIXED_3 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_SENDER_MIXED_3]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_SENDER_MIXED_3 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( RTP.OPT_SENDER_MIXED_4 /root/2023-lab2-rtp-junglethorn/build/rtp_test_all [==[--gtest_filter=RTP.OPT_SENDER_MIXED_4]==] --gtest_also_run_disabled_tests)
set_tests_properties( RTP.OPT_SENDER_MIXED_4 PROPERTIES WORKING_DIRECTORY /root/2023-lab2-rtp-junglethorn/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( rtp_test_all_TESTS RTP.NORMAL RTP.NORMAL_SMALL_WINDOW RTP.NORMAL_OPT RTP.NORMAL_OPT_SMALL_WINDOW RTP.RECEIVER_SINGLE_1 RTP.RECEIVER_SINGLE_2 RTP.RECEIVER_MIXED_1 RTP.RECEIVER_MIXED_2 RTP.SENDER_SINGLE_1 RTP.SENDER_SINGLE_2 RTP.SENDER_MIXED_1 RTP.SENDER_MIXED_2 RTP.OPT_RECEIVER_SINGLE_1 RTP.OPT_RECEIVER_SINGLE_2 RTP.OPT_RECEIVER_MIXED_1 RTP.OPT_RECEIVER_MIXED_2 RTP.OPT_SENDER_SINGLE_1 RTP.OPT_SENDER_SINGLE_2 RTP.OPT_SENDER_MIXED_1 RTP.OPT_SENDER_MIXED_2 RTP.NORMAL_HUGE_WINDOW RTP.NORMAL_OPT_HUGE_WINDOW RTP.OPT_RECEIVER_MIXED_3 RTP.OPT_RECEIVER_MIXED_4 RTP.OPT_SENDER_MIXED_3 RTP.OPT_SENDER_MIXED_4)
