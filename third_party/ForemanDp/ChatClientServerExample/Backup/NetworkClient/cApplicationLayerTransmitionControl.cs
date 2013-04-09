using System;
using System.Collections.Generic;
using System.Text;
//using ByteArrayOperations;

namespace NetworkClient
{
    /// <summary>
    /// This class enforce te following rules over data.
    /// [prefix length of(P)][P=prefix length of(data)][data...]
    /// </summary>
    public static class ApplicationLayerTransmitionControl
    {
        public static byte[] AddPrefix(byte[] Data)
        {
            byte[] bytArrDataLength = ByteArrayOperations.ConvertLongToByteArray((long)Data.Length);
            byte[] bytArrDataLengthPrefix = new byte[1] { (byte)bytArrDataLength.Length };
            byte[] NewData = new byte[bytArrDataLengthPrefix.Length + bytArrDataLength.Length + Data.Length];
            bytArrDataLengthPrefix.CopyTo(NewData, 0);
            bytArrDataLength.CopyTo(NewData, bytArrDataLengthPrefix.Length);
            Data.CopyTo(NewData, bytArrDataLengthPrefix.Length + bytArrDataLength.Length);
            return NewData;
        }

        public static bool IsIntegratedDataWaiting(byte[] Data)
        {
            bool bIntegratedDataWaiting = false;
            if (Data.Length > 1)
            {
                byte bytDataLengthPrefix = Data[0];
                if (Data.Length > bytDataLengthPrefix)
                {
                    byte[] bytArrDataLength = 
                        ByteArrayOperations.CopyFromByteArray(Data, 1, bytDataLengthPrefix);
                    long lDataLength = 
                        ByteArrayOperations.ConvertByteArrayToLong(bytArrDataLength);
                    if (lDataLength <= Data.Length - (bytArrDataLength.Length + 1))
                        bIntegratedDataWaiting = true;
                }
            }
            return bIntegratedDataWaiting;
        }

        public static byte[] ExtractIntegratedDataFromBuffer(ref byte[] Data)
        {
            byte[] integratedData = null;
            if (IsIntegratedDataWaiting(Data))
            {
                byte bytDataLengthPrefix = Data[0];
                byte[] bytArrDataLength = 
                    ByteArrayOperations.CopyFromByteArray(Data, 1, bytDataLengthPrefix);
                long lDataLength =
                    ByteArrayOperations.ConvertByteArrayToLong(bytArrDataLength);
                integratedData =
                    ByteArrayOperations.CopyFromByteArray(Data, bytArrDataLength.Length + 1, lDataLength);
                Data =
                    ByteArrayOperations.RemoveFromByteArray(Data, 0, 1 + bytArrDataLength.Length + lDataLength);
            }
            return integratedData;
        }
    }
}
