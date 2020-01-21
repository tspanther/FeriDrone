using stm_data_compression.Helpers;

namespace stm_data_compression
{
    class Program
    {
        static void Main(string[] args)
        {
            // Compress
            var compression = new Compression(isTesting: false);
            compression.Compress();

            // Decompress
            var decompression = new Decompression();
            decompression.Decompress();
        }
    }
}
