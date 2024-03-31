

BL_Err_t Reader_Start(void)
{
}

BL_Err_t Reader_ReadData(BL_UINT8_T *data, BL_UINT32_T *length)
{
    BL_Err_t err = writer.flags.write ? BL_ERR : BL_EINVAL;
    if (data && length && writer.flags.start && err == BL_EINVAL)
    {
        writer.flags.read = FLAG_SET;
        err = NVM_Read(writer.node, data, length);
    }
    return err;
}


