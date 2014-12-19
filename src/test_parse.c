#include <CUnit/Basic.h>

#include "parse.h"

#define BUFFER_SIZE 4096
#define DOMAIN "some-user@domain.ru"
#define DOMAIN_LEN (sizeof(DOMAIN) - 1)

static buffer_t buffer;

static int init_suite()
{
    return buffer_init(&buffer, BUFFER_SIZE);
}

static int clean_suite()
{
    buffer_destroy(&buffer);
    return 0;
}

#define POSITIVE_TEST(string, function) \
    { \
        buffer_shift_read(&buffer, buffer_left(&buffer)); \
        buffer_drop_read(&buffer); \
        buffer_write_string(&buffer, string); \
        size_t length; \
        const char *result = function(&buffer, &length); \
        CU_ASSERT_NOT_EQUAL_FATAL(result, NULL); \
        CU_ASSERT_EQUAL_FATAL(length, DOMAIN_LEN); \
        CU_ASSERT_EQUAL(strncmp(result, DOMAIN, DOMAIN_LEN), 0); \
    }

#define NEGATIVE_TEST(string, function) \
    { \
        buffer_shift_read(&buffer, buffer_left(&buffer)); \
        buffer_drop_read(&buffer); \
        buffer_write_string(&buffer, string); \
        size_t length; \
        const char *result = function(&buffer, &length); \
        CU_ASSERT_EQUAL(result, NULL); \
    }

static void test_parse_ehlo_lower_case_should_succeed()
{
    POSITIVE_TEST("ehlo " DOMAIN "\r\n", parse_ehlo_helo);
}

static void test_parse_ehlo_upper_case_should_succeed()
{
    POSITIVE_TEST("EHLO " DOMAIN "\r\n", parse_ehlo_helo);
}

static void test_parse_ehlo_mixed_case_should_succeed()
{
    POSITIVE_TEST("EhLo " DOMAIN "\r\n", parse_ehlo_helo);
}

static void test_parse_ehlo_with_additional_spaces_should_succeed()
{
    POSITIVE_TEST("ehlo   " DOMAIN "   \r\n", parse_ehlo_helo);
}

static void test_parse_ehlo_without_domain_should_return_null()
{
    NEGATIVE_TEST("ehlo\r\n", parse_ehlo_helo);
}

static void test_parse_ehlo_without_crlf_should_return_null()
{
    NEGATIVE_TEST("ehlo " DOMAIN, parse_ehlo_helo);
}

static void test_parse_ehlo_empty_should_return_null()
{
    NEGATIVE_TEST("", parse_ehlo_helo);
}

static void test_parse_helo_lower_case_should_succeed()
{
    POSITIVE_TEST("helo " DOMAIN "\r\n", parse_ehlo_helo);
}

static void test_parse_mail_lower_case_should_succeed()
{
    POSITIVE_TEST("mail from:<" DOMAIN ">\r\n", parse_mail);
}

static void test_parse_mail_upper_case_should_succeed()
{
    POSITIVE_TEST("MAIL FROM:<" DOMAIN ">\r\n", parse_mail);
}

static void test_parse_mail_mixed_case_should_succeed()
{
    POSITIVE_TEST("MaIl fRoM:<" DOMAIN ">\r\n", parse_mail);
}

static void test_parse_mail_with_additional_spaces_should_succeed()
{
    POSITIVE_TEST("mail from:   <" DOMAIN ">   \r\n", parse_mail);
}

static void test_parse_mail_without_domain_should_return_null()
{
    NEGATIVE_TEST("mail from:\r\n", parse_ehlo_helo);
}

static void test_parse_mail_with_empty_domain_should_return_null()
{
    NEGATIVE_TEST("mail from:<>\r\n", parse_ehlo_helo);
}

static void test_parse_mail_without_crlf_should_return_null()
{
    NEGATIVE_TEST("mail from:<" DOMAIN ">", parse_mail);
}

static void test_parse_mail_empty_should_return_null()
{
    NEGATIVE_TEST("", parse_mail);
}

static void test_parse_rcpt_lower_case_should_succeed()
{
    POSITIVE_TEST("rcpt to:<" DOMAIN ">\r\n", parse_rcpt);
}

static void test_parse_rcpt_upper_case_should_succeed()
{
    POSITIVE_TEST("RCPT to:<" DOMAIN ">\r\n", parse_rcpt);
}

static void test_parse_rcpt_mixed_case_should_succeed()
{
    POSITIVE_TEST("RcPt tO:<" DOMAIN ">\r\n", parse_rcpt);
}

static void test_parse_rcpt_with_additional_spaces_should_succeed()
{
    POSITIVE_TEST("rcpt to:   <" DOMAIN ">   \r\n", parse_rcpt);
}

static void test_parse_rcpt_without_domain_should_return_null()
{
    NEGATIVE_TEST("rcpt to:\r\n", parse_ehlo_helo);
}

static void test_parse_rcpt_with_empty_domain_should_return_null()
{
    NEGATIVE_TEST("rcpt to:<>\r\n", parse_ehlo_helo);
}

static void test_parse_rcpt_without_crlf_should_return_null()
{
    NEGATIVE_TEST("rcpt to:<" DOMAIN ">", parse_rcpt);
}

static void test_parse_rcpt_empty_should_return_null()
{
    NEGATIVE_TEST("", parse_rcpt);
}

#define INIT_SUITE(suite) \
    CU_pSuite suite = CU_add_suite(#suite, init_suite, clean_suite); \
    if (NULL == suite) { \
       CU_cleanup_registry(); \
       return CU_get_error(); \
    }

#define ADD_TEST(suite, test) \
    if (NULL == CU_add_test(suite, #test, test)) { \
        CU_cleanup_registry(); \
        return CU_get_error(); \
    }

int main()
{
   if (CUE_SUCCESS != CU_initialize_registry()) {
      return CU_get_error();
   }

   INIT_SUITE(parse_ehlo);
   ADD_TEST(parse_ehlo, test_parse_ehlo_lower_case_should_succeed);
   ADD_TEST(parse_ehlo, test_parse_ehlo_upper_case_should_succeed);
   ADD_TEST(parse_ehlo, test_parse_ehlo_mixed_case_should_succeed)
   ADD_TEST(parse_ehlo, test_parse_ehlo_with_additional_spaces_should_succeed);
   ADD_TEST(parse_ehlo, test_parse_ehlo_without_domain_should_return_null);
   ADD_TEST(parse_ehlo, test_parse_ehlo_without_crlf_should_return_null);
   ADD_TEST(parse_ehlo, test_parse_ehlo_empty_should_return_null);

   INIT_SUITE(parse_helo);
   ADD_TEST(parse_helo, test_parse_helo_lower_case_should_succeed);

   INIT_SUITE(parse_mail);
   ADD_TEST(parse_mail, test_parse_mail_lower_case_should_succeed);
   ADD_TEST(parse_mail, test_parse_mail_upper_case_should_succeed);
   ADD_TEST(parse_mail, test_parse_mail_mixed_case_should_succeed);
   ADD_TEST(parse_mail, test_parse_mail_with_additional_spaces_should_succeed);
   ADD_TEST(parse_mail, test_parse_mail_without_domain_should_return_null);
   ADD_TEST(parse_mail, test_parse_mail_with_empty_domain_should_return_null);
   ADD_TEST(parse_mail, test_parse_mail_without_crlf_should_return_null);
   ADD_TEST(parse_ehlo, test_parse_mail_empty_should_return_null);

   INIT_SUITE(parse_rcpt);
   ADD_TEST(parse_rcpt, test_parse_rcpt_lower_case_should_succeed);
   ADD_TEST(parse_rcpt, test_parse_rcpt_upper_case_should_succeed);
   ADD_TEST(parse_rcpt, test_parse_rcpt_mixed_case_should_succeed);
   ADD_TEST(parse_rcpt, test_parse_rcpt_with_additional_spaces_should_succeed);
   ADD_TEST(parse_rcpt, test_parse_rcpt_without_domain_should_return_null);
   ADD_TEST(parse_rcpt, test_parse_rcpt_with_empty_domain_should_return_null);
   ADD_TEST(parse_rcpt, test_parse_rcpt_without_crlf_should_return_null);
   ADD_TEST(parse_ehlo, test_parse_rcpt_empty_should_return_null);

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
