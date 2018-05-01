struct yaz0_header {
    char	magic[4];		// always "Yaz0"
    u32		uncompressed_size;	// total size of uncompressed data
    char	padding[8];		// always 0?
};
