using System;
using System.Collections.Generic;
using System.Text;

public static class ByteArrayOperations
{
    public static byte[] AddByteArrays(byte[] ByteArray1, byte[] ByteArray2)
    {
        if (ByteArray1 == null) ByteArray1 = new byte[0];
        if (ByteArray2 == null) ByteArray2 = new byte[0];
        byte[] ByteArrayResult = new byte[ByteArray1.Length + ByteArray2.Length];
        ByteArray1.CopyTo(ByteArrayResult, 0);
        ByteArray2.CopyTo(ByteArrayResult, ByteArray1.Length);
        return ByteArrayResult;
    }

    public static byte[] CopyFromByteArray(byte[] SourceArray, long StartIndex, long Count)
    {
        byte[] DestinationArray = new byte[Count];
        for (long lIndex = 0; lIndex < Count; lIndex++) DestinationArray[lIndex] = SourceArray[StartIndex + lIndex];
        return DestinationArray;
    }

    public static byte[] RemoveFromByteArray(byte[] SourceArray, long StartIndex, long Count)
    {
        long lIndex, lScan;
        byte[] DestinationArray = new byte[SourceArray.Length - Count];
        for (lIndex = 0; lIndex < StartIndex; lIndex++) // Copy from 0 to StartIndex
            DestinationArray[lIndex] = SourceArray[lIndex];
        for (lScan = StartIndex + Count; lScan < SourceArray.Length; lScan++, lIndex++) // Copy From StartIndex+Count to end
            DestinationArray[lIndex] = SourceArray[lScan];
        return DestinationArray;
    }

    public static byte[] ConvertLongToByteArray(long Number)
    {
        List<byte> bytes = new List<byte>();
        while (Number / 256 > 0)
        {
            bytes.Add((byte)(Number % 256));
            Number /= 256;
        }
        bytes.Add((byte)Number);
        byte[] arrBytes = new byte[bytes.Count];
        bytes.CopyTo(arrBytes);
        return arrBytes;
    }

    public static long ConvertByteArrayToLong(byte[] Data)
    {
        long Number = 0;
        for (byte iIndex = 0; iIndex < Data.Length; iIndex++)
            Number += Data[iIndex] * (long)(Math.Pow(256, (double)iIndex));
        return Number;
    }
}

