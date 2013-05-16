/* libsha.c */
void _sha1_init(sha1sum_t *sha1sum);
void _sha1_compute(sha1sum_t *sha1sum);
void _sha1_compute_final(sha1sum_t *sha1sum,
                         uint a, uint b, uint c, uint d, uint e);
void _sha1_normalize_endian(sha1sum_t *sha1sum);

void sha1_get_hex(char *hex, sha1sum_t *sha1sum);

/* libsha_test.c */
void _sha1_dump_block_of_W(sha1sum_t *sha1sum);
void _sha1_dump_ABCDE_header(void);
void _sha1_dump_ABCDE(int t, uint a, uint b, uint c, uint d, uint e);
