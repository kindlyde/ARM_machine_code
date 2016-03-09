struct tag_header {
	u32 size;
	u32 tag;
};

struct tag_mem32 {
	u32	size;
	u32	start;	/* physical start address */
};

struct tag_core {
	u32 flags;		/* bit 0 = read-only */
	u32 pagesize;
	u32 rootdev;
};

struct tag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core		core;
		struct tag_mem32	mem;
		struct tag_cmdline	cmdline;

	} u;
};

#define tag_next(t)	((struct tag *)((u32 *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)
#define ATAG_CORE	0x54410001
#define ATAG_MEM	0x54410002
#define ATAG_CMDLINE	0x54410009
#define ATAG_NONE	0x00000000