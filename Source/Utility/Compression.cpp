#include "Compression.h"

CCompression compression;

bool CCompression::Decompress(
	const uint8_t* compressed,
	size_t compressedLen,
	std::vector<uint8_t>& out,
	int windowBits /*= MAX_WBITS*/)
{
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.next_in = const_cast<Bytef*>(compressed);
	strm.avail_in = (uInt)compressedLen;

	if (inflateInit2(&strm, windowBits) != Z_OK)
		return false;

	const size_t CHUNK = 64 * 1024;
	out.clear();

	int ret = Z_OK;

	do
	{
		size_t offset = out.size();
		out.resize(offset + CHUNK);
		strm.next_out = out.data() + offset;
		strm.avail_out = (uInt)CHUNK;
		ret = inflate(&strm, Z_NO_FLUSH);

		if (ret != Z_OK && ret != Z_STREAM_END)
		{
			inflateEnd(&strm);
			out.clear();
			return false;
		}

		out.resize(offset + (CHUNK - strm.avail_out));
	} while (ret != Z_STREAM_END);

	inflateEnd(&strm);
	return true;
}
