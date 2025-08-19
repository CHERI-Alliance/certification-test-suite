#pragma once

#define cheri_address_get(x) __builtin_cheri_address_get(x)
#define cheri_address_set(x, y) __builtin_cheri_address_set((x), (y))
#define cheri_address_increment(x, y) __builtin_cheri_offset_increment((x), (y))
#define cheri_base_get(x) __builtin_cheri_base_get(x)
#define cheri_top_get(x) __builtin_cheri_top_get(x)
#define cheri_length_get(x) __builtin_cheri_length_get(x)
#define cheri_tag_clear(x) __builtin_cheri_tag_clear(x)
#define cheri_tag_get(x) __builtin_cheri_tag_get(x)
#define cheri_is_valid(x) __builtin_cheri_tag_get(x)
#define cheri_is_invalid(x) (!__builtin_cheri_tag_get(x))
#define cheri_is_equal_exact(x, y) __builtin_cheri_equal_exact((x), (y))
#define cheri_is_subset(x, y) __builtin_cheri_subset_test((x), (y))
#define cheri_permissions_get(x) __builtin_cheri_perms_get(x)
#define cheri_permissions_and(x, y) __builtin_cheri_perms_and((x), (y))
#define cheri_type_get(x) __builtin_cheri_type_get(x)
#define cheri_seal(a, b)                                                       \
	(CHERI_SEALED((__typeof__((a)))) __builtin_cheri_seal((a), (b)))
#define cheri_unseal(a, b)                                                     \
	((__typeof__(*(a)) *)__builtin_cheri_unseal((a), (b)))
#define cheri_bounds_set(a, b) __builtin_cheri_bounds_set((a), (b))
#define cheri_bounds_set_exact(a, b) __builtin_cheri_bounds_set_exact((a), (b))
#define cheri_subset_test(a, b) __builtin_cheri_subset_test(a, b)
#define cheri_representable_alignment_mask(len)                                \
	__builtin_cheri_representable_alignment_mask(len)
#define cheri_round_representable_length(len)                                  \
	__builtin_cheri_round_representable_length(len)
