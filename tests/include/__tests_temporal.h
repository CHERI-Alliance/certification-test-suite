#ifndef EXCLUDE_TEMPORAL_SAFETY
DEFINE_TEST(lazy_free, "use-after-reuse protection")
#	ifndef EXCLUDE_USE_AFTER_FREE
DEFINE_TEST(eager_free, "use-after-free protection")
#	endif
#endif
