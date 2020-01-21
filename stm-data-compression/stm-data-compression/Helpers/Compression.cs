using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using stm_data_compression.Utility;

namespace stm_data_compression.Helpers
{
    public class Compression
    {
        private List<int> _TestInput = new List<int> { 55, 53, 53, 53, 53, 53, 10, 10, 11, 11, 11, 11, };

        public Compression(
            string inputPath = null,
            string outputPath = null, 
            bool isTesting = false)
        {
            Bits2Reflection = new Dictionary<int, int>
            {
                { -2, 0},
                { -1, 1},
                { 1, 2},
                { 2, 3},
            };

            Bits3Reflection = new Dictionary<int, int>
            {
                { -6, 0},
                { -5, 1},
                { -4, 2},
                { -3, 3},
                { 3, 4},
                { 4, 5},
                { 5, 6},
                { 6, 7},
            };

            Bits4Reflection = new Dictionary<int, int>
            {
                { -14, 0 },
                { -13, 1 },
                { -12, 2 },
                { -11, 3 },
                { -10, 4 },
                { -9, 5 },
                { -8, 6 },
                { -7, 7 },
                { 7, 8 },
                { 8, 9 },
                { 9, 10 },
                { 10, 11 },
                { 11, 12 },
                { 12, 13 },
                { 13, 14 },
                { 14, 15 },
            };

            Bits5Reflection = new Dictionary<int, int>
            {
                { -30, 0 },
                { -29, 1 },
                { -28, 2 },
                { -27, 3 },
                { -26, 4 },
                { -25, 5 },
                { -24, 6 },
                { -23, 7 },
                { -22, 8 },
                { -21, 9 },
                { -20, 10 },
                { -19, 11 },
                { -18, 12 },
                { -17, 13 },
                { -16, 14 },
                { -15, 15 },
                { 15, 16 },
                { 16, 17 },
                { 17, 18 },
                { 18, 19 },
                { 19, 20 },
                { 20, 21 },
                { 21, 22 },
                { 22, 23 },
                { 23, 24 },
                { 24, 25 },
                { 25, 26 },
                { 26, 27 },
                { 27, 28 },
                { 28, 29 },
                { 29, 30 },
                { 30, 31 },
            };

            if (string.IsNullOrEmpty(inputPath)) 
            {
                InputPath = "input.csv";
            }

            if (string.IsNullOrEmpty(outputPath)) 
            {
                OutputPath = Constants.DefaultPathFile;
            }

            IsTesting = isTesting;
        }

        private Dictionary<int, int> Bits2Reflection { get; set; }

        private Dictionary<int, int> Bits3Reflection { get; set; }

        private Dictionary<int, int> Bits4Reflection { get; set; }

        private Dictionary<int, int> Bits5Reflection { get; set; }

        private string InputPath { get; set; }

        private string OutputPath { get; set; }

        private bool IsTesting { get; set; }

        public void Compress()
        {
            List<int> numbers;
            if (IsTesting) 
            {
                numbers = _TestInput;
            }
            else 
            {
                numbers = ReadData();
            }

            // If there is any issue with parsing csv file, we can compre this file and decompression output (because this is actual pre to post compression)
            //RedirectInputToFile(numbers);

            var compressedData = InternalCompress(numbers);

            var bytesData = ConvertBoolListToByteArray(compressedData);

            WriteOutput(bytesData);
        }

        private void WriteOutput(List<byte> input) 
        {
            File.WriteAllBytes(OutputPath, input.ToArray());
        }

        private List<byte> ConvertBoolListToByteArray(List<bool> inputList) 
        {
            var result = new List<byte>();

            int val = 0;
            var count = 0;
            foreach (var el in inputList) 
            {
                if (el) 
                {
                    val |= 1 << count;
                }

                count++;

                if (count == 8) 
                {
                    result.Add((byte)val);

                    val = 0;
                    count = 0;
                }
            }

            return result;
        }

        private List<bool> InternalCompress(List<int> input) 
        {
            var result = new List<bool>();

            // Encode first number with 8 bits
            var firstNumberBits = IntToBits(input.First(), 8);

            result.AddRange(firstNumberBits);

            var zeroCount = 0;

            for (var i = 0; i < input.Count; i++)
            {
                if (i == input.Count - 1) 
                {
                    // Flush out remaining zeroes (if any)
                    if (zeroCount != 0)
                    {
                        result.AddRange(new List<bool> {
                            false,
                            true,
                        });

                        var finalZeroBits = IntToBits(zeroCount - 1, 3);

                        result.AddRange(finalZeroBits);
                    }

                    // Add 11, meaning, end of input.
                    result.AddRange(new List<bool> 
                    { 
                        true, 
                        true,
                    });

                    break;
                }

                var difference = input[i + 1] - input[i];

                if (difference == 0)
                {
                    // count of zeroes for encoding of type 
                    zeroCount++;

                    if (zeroCount == 8) 
                    {
                        // First, write 01
                        result.AddRange(new List<bool> 
                        { 
                            false, 
                            true,
                        });

                        // Flush 111 to output as 3 bits (this represents number 8, because 000 represents 1).
                        var zeroBits = IntToBits(7, 3);

                        result.AddRange(zeroBits);

                        zeroCount = 0;
                    }
                }
                else 
                {
                    // First, Flush option
                    if (zeroCount != 0) 
                    {
                        // First, write 01
                        result.AddRange(new List<bool> 
                        {
                            false, 
                            true,
                        });

                        // Write zeroes
                        var zeroBits = IntToBits(zeroCount - 1, 3);

                        result.AddRange(zeroBits);

                        zeroCount = 0;
                    }

                    // Next, it is either options 00 or 10
                    if (Math.Abs(difference) > 30)
                    {
                        // Options 10, absolute coding
                        result.AddRange(new List<bool> {
                            true,
                            false,
                        });

                        // First bit is sign
                        if (difference < 0)
                        {
                            result.Add(true); // 1 means negative number
                        }
                        else
                        {
                            result.Add(false); // 0 means positive number
                        }

                        difference = Math.Abs(difference);

                        // Number will be encoded with 8 bits.
                        var differenceBits = IntToBits(difference, 8);

                        result.AddRange(differenceBits);
                    }
                    else
                    {
                        // Option 00
                        result.AddRange(new List<bool> {
                            false,
                            false,
                        });

                        // Get value to encode from bit dictionaries
                        if (difference >= -2 && difference <= 2)
                        {
                            // Write 00
                            result.AddRange(new List<bool>
                            {
                                false,
                                false,
                            });

                            // Write 2 bit number
                            var _2bitDifferenceBits = IntToBits(
                                Bits2Reflection[difference],
                                2);

                            result.AddRange(_2bitDifferenceBits);
                        }
                        else if (difference >= -6 && difference <= 6)
                        {
                            // write 01
                            result.AddRange(new List<bool>
                            {
                                false,
                                true,
                            });

                            // Write 3 bit difference
                            var _3bitDifferenceBits = IntToBits(
                                Bits3Reflection[difference],
                                3);

                            result.AddRange(_3bitDifferenceBits);
                        }
                        else if (difference >= -14 && difference <= 14)
                        {
                            // Write 10
                            result.AddRange(new List<bool>
                            {
                                true,
                                false,
                            });

                            // Write 4 bits
                            var _4bitDifferenceBits = IntToBits(
                                Bits4Reflection[difference],
                                4);

                            result.AddRange(_4bitDifferenceBits);
                        }
                        else if (difference >= -30 && difference <= 30) 
                        {
                            // Write 11
                            result.AddRange(new List<bool> 
                            {
                                true, 
                                true,
                            });

                            // Write 5 bits
                            var _5bitDifferenceBits = IntToBits(
                                Bits5Reflection[difference],
                                5);

                            result.AddRange(_5bitDifferenceBits);
                        }
                    }
                }
            }

            // Ensure length is dividable with 0
            while (true)
            {
                if (result.Count % 8 == 0) 
                {
                    break;
                }

                result.Add(false);
            }

            return result;
        }

        private List<bool> IntToBits(int input,  int? expectedLength)
        {
            var result = new List<bool>();

            var divided = input;

            while (true)
            {
                if (divided == 0)
                {
                    break;
                }

                var remainder = divided % 2;

                if (remainder == 0)
                {
                    result.Insert(0, false);
                }
                else
                {
                    result.Insert(0, true);
                }

                divided >>= 1;
            }

            if (expectedLength.HasValue) 
            {
                while (true)
                {
                    if (result.Count >= expectedLength.Value)
                    {
                        break;
                    }

                    result.Insert(0, false);
                }
            }

            return result;
        }

        private List<int> ReadData() 
        {
            var lines = File
                .ReadLines(InputPath)
                .ToList();

            if (lines == null || lines.Count() == 0) 
            {
                return null;
            }

            lines = lines.GetRange(1, lines.Count - 1);

            var floatValus = new List<float>();

            for (var i= 0; i<lines.Count; i++)
            {
                var line = lines[i];

                var stringNumbers = line
                    .Split(',')
                    .ToList();

                if (stringNumbers.Count == 0)
                {
                    continue;
                }

                stringNumbers.RemoveAt(0);

                if (stringNumbers.Count != 6) 
                {
                    // Each line must contain exactly 7 numbers (first number is count number and then 6 values).
                    Console.WriteLine(
                        $"Input is not valid. Line {stringNumbers[0]} containts {stringNumbers.Count} numbers.");

                    continue;
                }

                var numbers = new List<float>();

                foreach (var stringNumber in stringNumbers) 
                {
                    var isFloat = float.TryParse(
                        stringNumber,
                        NumberStyles.Number | NumberStyles.AllowExponent,
                        CultureInfo.InvariantCulture,
                        out float number);

                    if (isFloat) 
                    {
                        numbers.Add(number);
                    }
                }

                if (numbers.Count == 6) 
                {
                    floatValus.AddRange(numbers);
                }
            }

            // Convert doubles to int
            // We determine what we do with rounding here... Do we .Round it to closest value, perform .Ceil or .Floor...
            var result = floatValus
                .Select(x => 
                {
                    return (int)Math.Round(x); 
                })
                .ToList();

            return result;
        }


        private void RedirectInputToFile(List<int> input)
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

            File.WriteAllText("input_pre_compression.txt", outText);
        }
    }
}
