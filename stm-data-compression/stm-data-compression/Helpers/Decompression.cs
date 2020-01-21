using System.Collections.Generic;
using System.IO;
using System.Linq;
using stm_data_compression.Utility;

namespace stm_data_compression.Helpers
{
    class Decompression
    {
        public Decompression(
            string inputPath = null,
            string outputPath = null)
        {
            Bits2Reflection = new Dictionary<int, int>
            {
                { 0, -2},
                { 1, -1},
                { 2, 1},
                { 3, 2},
            };

            Bits3Reflection = new Dictionary<int, int>
            {
                { 0, -6},
                { 1, -5},
                { 2, -4},
                { 3, -3},
                { 4, 3},
                { 5, 4},
                { 6, 5},
                { 7, 6},
            };

            Bits4Reflection = new Dictionary<int, int>
            {
                { 0, -14 },
                { 1, -13 },
                { 2, -12 },
                { 3, -11 },
                { 4, -10 },
                { 5, -9 },
                { 6, -8 },
                { 7, -7 },
                { 8, 7 },
                { 9, 8 },
                { 10, 9 },
                { 11, 10 },
                { 12, 11 },
                { 13, 12 },
                { 14, 13 },
                { 15, 14 },
            };

            Bits5Reflection = new Dictionary<int, int>
            {
                { 0, -30 },
                { 1, -29 },
                { 2, -28 },
                { 3, -27 },
                { 4, -26 },
                { 5, -25 },
                { 6, -24 },
                { 7, -23 },
                { 8, -22 },
                { 9, -21 },
                { 10, -20 },
                { 11, -19 },
                { 12, -18 },
                { 13, -17 },
                { 14, -16 },
                { 15, -15 },
                { 16, 15 },
                { 17, 16 },
                { 18, 17 },
                { 19, 18 },
                { 20, 19 },
                { 21, 20 },
                { 22, 21 },
                { 23, 22 },
                { 24, 23 },
                { 25, 24 },
                { 26, 25 },
                { 27, 26 },
                { 28, 27 },
                { 29, 28 },
                { 30, 29 },
                { 31, 30 },
            };

            if (string.IsNullOrEmpty(inputPath))
            {
                InputPath = Constants.DefaultPathFile;
            }

            if (string.IsNullOrEmpty(outputPath)) 
            {
                OutputPath = "input_compressed.txt";
            }
        }

        private Dictionary<int, int> Bits2Reflection { get; set; }

        private Dictionary<int, int> Bits3Reflection { get; set; }

        private Dictionary<int, int> Bits4Reflection { get; set; }

        private Dictionary<int, int> Bits5Reflection { get; set; }

        private string InputPath { get; set; }

        private string OutputPath { get; set; }

        public void Decompress() 
        {
            var input = ReadCompressedInput();

            var data = DecompressInternal(input);

            WriteOutput(data);
        }

        private void WriteOutput(List<int> input) 
        {
            var outText = "pos_x pos_y pos_z roll pitch yaw\r\n";
            
            var lineCount = 1;
            var numCount = 0;

            // reconstruct original data into a txt file (for potencial further processing)
            var line = string.Empty;
            for (var i = 0; i < input.Count; i++) 
            {
                if (numCount == 0) 
                {
                    line = $"{lineCount}";
                    numCount++;
                }

                line += $" {input[i]}";
                numCount++;

                if (numCount == 7) 
                {
                    line += "\r\n";

                    outText += line;
                    lineCount++;
                    numCount = 0;
                }
            }

            File.WriteAllText(OutputPath, outText);
        }

        private List<int> DecompressInternal(List<bool> input) 
        {
            var differences = new List<int>();

            // Read first number (8 bits)
            var firstNumberBits = input.GetRange(0, 8);
            var firstNumber = BooleanListToInt(firstNumberBits);

            differences.Add(firstNumber);

            // We have already read the first 8 bits.
            var position = 8;

            while (true)
            {
                var headerBits = input.GetRange(position, 2);
                position += 2;

                if (headerBits[0] == false && headerBits[1] == false)
                {
                    // 00 coding

                    // read 2 more bits, so we know which dictionary to use.
                    var dictionaryBitReflection = input.GetRange(position, 2);
                    position += 2;

                    if (dictionaryBitReflection[0] == false && dictionaryBitReflection[1] == false) 
                    {
                        var number2IndexBits = input.GetRange(position, 2);
                        position += 2;

                        var number2Index = BooleanListToInt(number2IndexBits);

                        var num = Bits2Reflection[number2Index];

                        differences.Add(num);
                    }
                    else if (dictionaryBitReflection[0] == false && dictionaryBitReflection[1] == true)
                    {
                        var number3IndexBits = input.GetRange(position, 3);
                        position += 3;

                        var number3Index = BooleanListToInt(number3IndexBits);

                        var num = Bits3Reflection[number3Index];

                        differences.Add(num);
                    }
                    else if (dictionaryBitReflection[0] == true && dictionaryBitReflection[1] == false)
                    {
                        var number4IndexBits = input.GetRange(position, 4);
                        position += 4;

                        var number4Index = BooleanListToInt(number4IndexBits);

                        var num = Bits4Reflection[number4Index];

                        differences.Add(num);
                    }
                    else if (dictionaryBitReflection[0] == true && dictionaryBitReflection[1] == true)
                    {
                        var number5IndexBits = input.GetRange(position, 5);
                        position += 5;

                        var number5Index = BooleanListToInt(number5IndexBits);

                        var num = Bits5Reflection[number5Index];

                        differences.Add(num);
                    }
                }
                else if (headerBits[0] == false && headerBits[1] == true)
                {
                    // 01 coding

                    var zeroCountBits = input.GetRange(position, 3);
                    position += 3;
                    // +1 here is because 000 encodes 1 zero number (meaning, everything is 'skewed' by +1, 111 represents 8 and not 7).
                    var zeroCount = BooleanListToInt(zeroCountBits) + 1;

                    for (var i = 0; i < zeroCount; i++) 
                    {
                        differences.Add(0);
                    }
                }
                else if (headerBits[0] == true && headerBits[1] == false)
                {
                    // 10 encoding

                    var signBit = input.GetRange(position, 1).First();
                    position++;

                    var absoluteNumberBits = input.GetRange(position, 8);
                    var absoluteNum = BooleanListToInt(absoluteNumberBits);
                    position += 8;

                    // if signBit is true, then its a negative number.
                    if (signBit) 
                    {
                        absoluteNum = -absoluteNum;
                    }

                    differences.Add(absoluteNum);

                }
                else if (headerBits[0] == true && headerBits[1] == true)
                {
                    // 11 encoding

                    break;
                }
            }

            // Now reconstruct array of values from differences array.

            var result = new List<int>();

            result.Add(firstNumber);

            var current = firstNumber;
            for (var i = 0; i < differences.Count - 1; i++) 
            {
                var newCurrent = current + differences[i + 1];

                result.Add(newCurrent);

                current = newCurrent;
            }

            return result;
        }

        private int BooleanListToInt(List<bool> input) 
        {
            // Flip bits, because increment of 2 goes from Right->Left.
            input.Reverse();

            var result = 0;

            for (int i = 0; i < input.Count; i++) 
            {
                if (input[i]) 
                {
                    result = result + (1 << i);
                }
            }

            return result;
        }

        private List<bool> ByteArrayToBooleanList(byte[] input) 
        {
            var result = new List<bool>();

            foreach (var el in input) 
            {
                for (var i = 0; i < 8; i++) 
                {
                    var bit = (el & (1 << i));

                    if (bit == 0)
                    {
                        result.Add(false);
                    }
                    else 
                    {
                        result.Add(true);
                    }
                }
            }

            return result;
        }

        private List<bool> ReadCompressedInput() 
        {
            var bytes = File.ReadAllBytes(InputPath);

            var result = ByteArrayToBooleanList(bytes);

            return result;
        }
    }
}
