#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/memory.hlsli"

struct EmptyCallable
{
    void operator()(int, int)
    {
    }
};

[numthreads(1, 1, 1)]
void SectionHierarchyByteWriterTests()
{
    EmptyCallable nullCallable;
    SCENARIO("")
    {
        stf::detail::PerThreadScratchData data;
        ttl::zero(data);

        uint vec[stf::detail::NumSections / 4];
        ttl::zero(vec);

        SECTION("GIVEN Uninitialized THEN Properties are as expected")
        {
            stf::detail::SectionHierarchy sh;
            sh.Scratch = data;
            ASSERT(AreEqual, ttl::bytes_required(sh), 0u);
            ASSERT(AreEqual, ttl::alignment_required(sh), 4u);
        }

        SECTION("GIVEN Initialized ")
        {
            data.Init();

            SECTION("THEN Properties are as expected")
            {
                stf::detail::SectionHierarchy sh;
                sh.Scratch = data;
                ASSERT(AreEqual, ttl::bytes_required(sh), 4u);
                ASSERT(AreEqual, ttl::alignment_required(sh), 4u);
            }

            data.TryLoopScenario(nullCallable);

            SECTION("WHEN Bytes Written")
            {
                stf::detail::SectionHierarchy sh;
                sh.Scratch = data;
                ttl::write_bytes(vec, 0, sh);

                SECTION("THEN first uint is 0")
                {
                    ASSERT(AreEqual, vec[0], 0u);
                }
            }

            SECTION("WHEN Section Entered")
            {
                data.TryEnterSection(nullCallable, 1);

                SECTION("THEN Properties are as expected")
                {
                    stf::detail::SectionHierarchy sh;
                    sh.Scratch = data;
                    ASSERT(AreEqual, ttl::bytes_required(sh), 4u);
                    ASSERT(AreEqual, ttl::alignment_required(sh), 4u);
                }

                SECTION("AND WHEN 3 sections entered")
                {
                    data.TryEnterSection(nullCallable, 2);
                    data.TryEnterSection(nullCallable, 3);

                    SECTION("THEN 4 Bytes still required")
                    {
                        stf::detail::SectionHierarchy sh;
                        sh.Scratch = data;
                        ASSERT(AreEqual, ttl::bytes_required(sh), 4u);
                        ASSERT(AreEqual, ttl::alignment_required(sh), 4u);
                    }

                    SECTION("AND WHEN bytes written")
                    {
                        stf::detail::SectionHierarchy sh;
                        sh.Scratch = data;
                        ttl::write_bytes(vec, 0, sh);

                        SECTION("THEN first uint is (0 << 24) | (1 << 16) | (2 << 8) | (3 << 0)")
                        {
                            static const uint expected = (0 << 24) | (1 << 16) | (2 << 8) | (3 << 0);
                            ASSERT(AreEqual, vec[0], expected);
                        }
                    }

                    SECTION("AND WHEN 5 sections entered")
                    {
                        data.TryEnterSection(nullCallable, 4);
                        data.TryEnterSection(nullCallable, 5);

                        SECTION("THEN 8 Bytes is required")
                        {
                            stf::detail::SectionHierarchy sh;
                            sh.Scratch = data;
                            ASSERT(AreEqual, ttl::bytes_required(sh), 8u);
                            ASSERT(AreEqual, ttl::alignment_required(sh), 4u);
                        }

                        SECTION("AND WHEN bytes written")
                        {
                            stf::detail::SectionHierarchy sh;
                            sh.Scratch = data;
                            ttl::write_bytes(vec, 0, sh);
                            
                            SECTION("THEN first two uints is as expected")
                            {
                                static const uint expectedFirst = (2 << 24) | (3 << 16) | (4 << 8) | (5 << 0);
                                static const uint expectedSecond = 1;
                                ASSERT(AreEqual, vec[0], expectedFirst);
                                ASSERT(AreEqual, vec[1], expectedSecond);
                            }
                        }

                        SECTION("AND WHEN two sections are left")
                        {
                            data.OnLeave();
                            data.OnLeave();

                            SECTION("THEN 4 Bytes now required")
                            {
                                stf::detail::SectionHierarchy sh;
                                sh.Scratch = data;
                                ASSERT(AreEqual, ttl::bytes_required(sh), 4u);
                                ASSERT(AreEqual, ttl::alignment_required(sh), 4u);
                            }

                            SECTION("AND WHEN bytes written")
                            {
                                stf::detail::SectionHierarchy sh;
                                sh.Scratch = data;
                                ttl::write_bytes(vec, 0, sh);

                                SECTION("THEN first uint is (0 << 24) | (1 << 16) | (2 << 8) | (3 << 0)")
                                {
                                    static const uint expected = (0 << 24) | (1 << 16) | (2 << 8) | (3 << 0);
                                    ASSERT(AreEqual, vec[0], expected);
                                }
                            }
                        }
                    }

                    SECTION("AND WHEN a section is left")
                    {
                        data.OnLeave();

                        SECTION("THEN 4 Bytes still required")
                        {
                            stf::detail::SectionHierarchy sh;
                            sh.Scratch = data;
                            ASSERT(AreEqual, ttl::bytes_required(sh), 4u);
                            ASSERT(AreEqual, ttl::alignment_required(sh), 4u);
                        }

                        SECTION("AND WHEN bytes written")
                        {
                            stf::detail::SectionHierarchy sh;
                            sh.Scratch = data;
                            ttl::write_bytes(vec, 0, sh);

                            SECTION("THEN first uint is (0 << 16) | (1 << 8) | (2 << 0)")
                            {
                                static const uint expected = (0 << 16) | (1 << 8) | (2 << 0);
                                ASSERT(AreEqual, vec[0], expected);
                            }
                        }
                    }
                }
            }
        }
    }
}
