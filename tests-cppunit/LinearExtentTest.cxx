#include <StrMod/LinearExtent.h>
#include <climits>
#include <cppunit/TestFixture.h>
#include <cppunit/SourceLine.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

namespace strmod {
namespace tests {

using ::strmod::strmod::LinearExtent;

#define ASSERT_SOURCELINE(condition, sourceline) \
      (::CppUnit::Asserter::failIf( !(condition),\
                                      (#condition), \
                                      (sourceline)))

#define ASSERT_EQUAL_SOURCELINE(expected, actual, sourceline) \
      (::CppUnit::TestAssert::assertEquals( (expected),\
                                             (actual), \
                                             (sourceline)))
#define ASSERT_EQUAL_MESSAGE_SOURCELINE(expected, actual, message, sourceline) \
      (::CppUnit::TestAssert::assertEquals( (expected),\
                                             (actual), \
                                             (sourceline), \
                                             (message)))

class LinearExtentTest : public ::CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE(LinearExtentTest);
   CPPUNIT_TEST(testConstructor);
   CPPUNIT_TEST(testStatic);
   CPPUNIT_TEST(testSetters);
   CPPUNIT_TEST(testAssign);
   CPPUNIT_TEST(testBeginEndOffset);
   CPPUNIT_TEST(testIntersection);
   CPPUNIT_TEST(testLengthen);
   CPPUNIT_TEST(testShorten);
   CPPUNIT_TEST(testMove);
   CPPUNIT_TEST(testSubExtent);
   CPPUNIT_TEST_SUITE_END();
 public:
   void testConstructor()
   {
      {
         LinearExtent ext;
         CPPUNIT_ASSERT_EQUAL(0U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(0U, ext.Length());
      }
      {
         LinearExtent ext(5, 10);
         CPPUNIT_ASSERT_EQUAL(5U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(10U, ext.Length());
      }
   }
   void testStatic()
   {
      CPPUNIT_ASSERT_EQUAL(0U, LinearExtent::full_extent.Offset());
      CPPUNIT_ASSERT_EQUAL(0U - 1U, LinearExtent::full_extent.Length());
   }
   void testSetters()
   {
      LinearExtent ext(5, 10);
      ext.Offset(15);
      CPPUNIT_ASSERT_EQUAL(15U, ext.Offset());
      CPPUNIT_ASSERT_EQUAL(10U, ext.Length());
      ext.Length(20);
      CPPUNIT_ASSERT_EQUAL(15U, ext.Offset());
      CPPUNIT_ASSERT_EQUAL(20U, ext.Length());
   }
   void testAssign()
   {
      LinearExtent ext1(5, 10);
      LinearExtent ext2(ext1);
      CPPUNIT_ASSERT_EQUAL(5U, ext1.Offset());
      CPPUNIT_ASSERT_EQUAL(10U, ext1.Length());
      CPPUNIT_ASSERT_EQUAL(ext1, ext2);
      ext2.Offset(15);
      ext2.Length(20);
      CPPUNIT_ASSERT_EQUAL(5U, ext1.Offset());
      CPPUNIT_ASSERT_EQUAL(10U, ext1.Length());
      CPPUNIT_ASSERT_EQUAL(15U, ext2.Offset());
      CPPUNIT_ASSERT_EQUAL(20U, ext2.Length());
      ext1 = ext2;
      CPPUNIT_ASSERT_EQUAL(ext1, ext2);
      ext2 = LinearExtent(30, 40);
      CPPUNIT_ASSERT_EQUAL(15U, ext1.Offset());
      CPPUNIT_ASSERT_EQUAL(20U, ext1.Length());
      CPPUNIT_ASSERT_EQUAL(30U, ext2.Offset());
      CPPUNIT_ASSERT_EQUAL(40U, ext2.Length());
   }
   void testBeginEndOffset()
   {
      LinearExtent ext(0, 20);
      CPPUNIT_ASSERT_EQUAL(0U, ext.beginOffset());
      CPPUNIT_ASSERT_EQUAL(20U, ext.endOffset());
      ext = LinearExtent(10, 30);
      CPPUNIT_ASSERT_EQUAL(10U, ext.beginOffset());
      CPPUNIT_ASSERT_EQUAL(40U, ext.endOffset());
   }
   void testIntersection()
   {
      LinearExtent ext1(0, 4);
      LinearExtent ext2(ext1);
      doIntserect(ext1, ext2, LinearExtent(0, 4), CPPUNIT_SOURCELINE());
      ext2.Offset(1);
      doIntserect(ext1, ext2, LinearExtent(1, 3), CPPUNIT_SOURCELINE());
      ext2.Offset(2);
      doIntserect(ext1, ext2, LinearExtent(2, 2), CPPUNIT_SOURCELINE());
      ext2.Offset(3);
      doIntserect(ext1, ext2, LinearExtent(3, 1), CPPUNIT_SOURCELINE());
      ext2.Offset(4);
      {
         const LinearExtent int1 = ext1.intersection(ext2);
         const LinearExtent int2 = ext2.intersection(ext1);
         CPPUNIT_ASSERT_EQUAL(int1, int2);
         CPPUNIT_ASSERT_EQUAL(0U, int1.Length());
      }
      ext2.Offset(5);
      {
         const LinearExtent int1 = ext1.intersection(ext2);
         const LinearExtent int2 = ext2.intersection(ext1);
         CPPUNIT_ASSERT_EQUAL(int1, int2);
         CPPUNIT_ASSERT_EQUAL(0U, int1.Length());
      }
   }
   void testLengthen()
   {
      {
         LinearExtent ext(5, 20);
         const LinearExtent::off_t end = ext.endOffset();
         ext.LengthenLeft(3);
         CPPUNIT_ASSERT_EQUAL(23U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
         ext.LengthenLeft(5);
         // 25 not 28 because there are only 2 left on the left side before the
         // beginning.
         CPPUNIT_ASSERT_EQUAL(25U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
      }
      {
         const LinearExtent::length_t maxlen = LinearExtent::LENGTH_MAX;
         LinearExtent ext(5, maxlen - 5U);
         const LinearExtent::off_t end = ext.endOffset();
         ext.LengthenLeft(3);
         CPPUNIT_ASSERT_EQUAL(maxlen - 2U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
         ext.LengthenLeft(5);
         // maxlen, not undefined because there are only 2 left on the left side
         // before the beginning.
         CPPUNIT_ASSERT_EQUAL(maxlen, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
      }
      {
         LinearExtent ext(5, 20);
         const LinearExtent::off_t begin = ext.beginOffset();
         ext.LengthenRight(3);
         CPPUNIT_ASSERT_EQUAL(23U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
         ext.LengthenRight(5);
         CPPUNIT_ASSERT_EQUAL(28U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
      }
      {
         const LinearExtent::length_t maxlen = LinearExtent::LENGTH_MAX;
         LinearExtent ext(0, maxlen - 5U);
         const LinearExtent::off_t begin = ext.beginOffset();
         ext.LengthenRight(3);
         CPPUNIT_ASSERT_EQUAL(maxlen - 2U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
         ext.LengthenRight(5);
         // maxlen, not undefined because there are only 2 left on the right side
         // before the end.
         CPPUNIT_ASSERT_EQUAL(maxlen, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
      }
   }
   void testShorten()
   {
      {
         LinearExtent ext(5, 20);
         const LinearExtent::off_t end = ext.endOffset();
         ext.ShortenLeft(3);
         CPPUNIT_ASSERT_EQUAL(17U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
         ext.ShortenLeft(18);
         CPPUNIT_ASSERT_EQUAL(0U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
      }
      {
         const LinearExtent::length_t maxlen = LinearExtent::LENGTH_MAX;
         LinearExtent ext(5, maxlen - 5U);
         const LinearExtent::off_t end = ext.endOffset();
         ext.ShortenLeft(3);
         CPPUNIT_ASSERT_EQUAL(maxlen - 8U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
         ext.ShortenLeft(maxlen);
         CPPUNIT_ASSERT_EQUAL(0U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(end, ext.endOffset());
      }
      {
         LinearExtent ext(5, 20);
         const LinearExtent::off_t begin = ext.beginOffset();
         ext.ShortenRight(3);
         CPPUNIT_ASSERT_EQUAL(17U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
         ext.ShortenRight(18);
         CPPUNIT_ASSERT_EQUAL(0U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
      }
      {
         const LinearExtent::length_t maxlen = LinearExtent::LENGTH_MAX;
         LinearExtent ext(0, maxlen - 5U);
         const LinearExtent::off_t begin = ext.beginOffset();
         ext.ShortenRight(3);
         CPPUNIT_ASSERT_EQUAL(maxlen - 8U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
         ext.ShortenRight(maxlen);
         CPPUNIT_ASSERT_EQUAL(0U, ext.Length());
         CPPUNIT_ASSERT_EQUAL(begin, ext.beginOffset());
      }
   }
   void testMove()
   {
      {
         LinearExtent ext(5, 20);
         const LinearExtent::length_t length = ext.Length();
         ext.MoveRight(3);
         CPPUNIT_ASSERT_EQUAL(8U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
         ext.MoveLeft(3);
         CPPUNIT_ASSERT_EQUAL(5U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
         const LinearExtent::length_t maxlen = LinearExtent::LENGTH_MAX;
         ext.MoveRight(maxlen);
         CPPUNIT_ASSERT_EQUAL(maxlen - 20U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
         ext.MoveLeft(maxlen);
         CPPUNIT_ASSERT_EQUAL(0U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
      }
      {
         const LinearExtent::length_t maxlen = LinearExtent::LENGTH_MAX;
         LinearExtent ext(5, maxlen - 7U);
         const LinearExtent::length_t length = ext.Length();
         ext.MoveRight(3);
         CPPUNIT_ASSERT_EQUAL(7U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
         ext.MoveLeft(3);
         CPPUNIT_ASSERT_EQUAL(4U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
         ext.MoveRight(maxlen);
         CPPUNIT_ASSERT_EQUAL(7U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
         ext.MoveLeft(maxlen);
         CPPUNIT_ASSERT_EQUAL(0U, ext.Offset());
         CPPUNIT_ASSERT_EQUAL(length, ext.Length());
      }
   }
   void testSubExtent()
   {
      LinearExtent ext1(0, 100);
      LinearExtent ext2(ext1);
      assertSubExtent(ext1, ext2, LinearExtent(0, 100), CPPUNIT_SOURCELINE());
      ext2 = LinearExtent::full_extent;
      assertSubExtent(ext1, ext2, LinearExtent(0, 100), CPPUNIT_SOURCELINE());
      assertSubExtent(ext2, ext1, LinearExtent(0, 100), CPPUNIT_SOURCELINE());
      ext1 = LinearExtent::full_extent;
      assertSubExtent(ext1, ext2, LinearExtent::full_extent, CPPUNIT_SOURCELINE());
      ext1 = LinearExtent(10, 100);
      ext2 = LinearExtent(10, 100);
      assertSubExtent(ext1, ext2, LinearExtent(20, 90), CPPUNIT_SOURCELINE());
      ext2 = LinearExtent::full_extent;
      assertSubExtent(ext1, ext2, LinearExtent(10, 100), CPPUNIT_SOURCELINE());
      assertSubExtent(ext2, ext1, LinearExtent(10, 100), CPPUNIT_SOURCELINE());
      ext2 = LinearExtent(100, 10);
      assertSubExtent(ext1, ext2, LinearExtent(110, 0), CPPUNIT_SOURCELINE());
      ext2 = LinearExtent(101, 10);
      assertSubExtent(ext1, ext2, LinearExtent(110, 0), CPPUNIT_SOURCELINE());
      ext2 = LinearExtent(120, 10);
      assertSubExtent(ext1, ext2, LinearExtent(110, 0), CPPUNIT_SOURCELINE());
      ext2 = LinearExtent(90, LinearExtent::LENGTH_MAX - 90);
      assertSubExtent(ext1, ext2, LinearExtent(100, 10), CPPUNIT_SOURCELINE());
      ext1 = ext2;
      ext2 = LinearExtent(10, LinearExtent::LENGTH_MAX - 10);
      assertSubExtent(ext1, ext2,
                      LinearExtent(100, LinearExtent::LENGTH_MAX - 100),
                      CPPUNIT_SOURCELINE());
   }

 private:
   void doIntserect(const LinearExtent &ext1, const LinearExtent &ext2,
                    const LinearExtent &expected_result,
                    const CppUnit::SourceLine &sourceline)
   {
      const LinearExtent int1 = ext1.intersection(ext2);
      const LinearExtent int2 = ext2.intersection(ext1);
      ASSERT_EQUAL_MESSAGE_SOURCELINE(int1, int2, "int1 == int2", sourceline);
      ASSERT_EQUAL_MESSAGE_SOURCELINE(expected_result, int1,
                                      "expected_result == int1", sourceline);
   }
   void assertSubExtent(const LinearExtent &ext1,
                        const LinearExtent &ext2,
                        const LinearExtent &expected_result,
                        const CppUnit::SourceLine &sourceline)
   {
      LinearExtent sub1 = ext1.SubExtent(ext2);
      LinearExtent sub2(ext1);
      sub2.SubExtent_eq(ext2);
      ASSERT_EQUAL_MESSAGE_SOURCELINE(sub1, sub2, "sub1 == sub2", sourceline);
      ASSERT_EQUAL_MESSAGE_SOURCELINE(expected_result, sub1,
                                      "expected_result == sub1", sourceline);
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinearExtentTest);

} // namespace strmod::tests
} // namespace strmod
