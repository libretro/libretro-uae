static int (*calltrap)(...) = (int (*)(...))0xF0FF60;

static int GetVersion(void)
{
    calltrap (0);
}
static int GetUaeConfig(struct UAE_CONFIG *a)
{
    calltrap (1, a);
}
static int SetUaeConfig(struct UAE_CONFIG *a)
{
    calltrap (2, a);
}
static int HardReset(void)
{
    calltrap (3);
}
static int Reset(void)
{
    calltrap (4);
}
static int EjectDisk(ULONG drive)
{
    calltrap (5, "", drive);
}
static int InsertDisk(UBYTE *name, ULONG drive)
{
    calltrap (5, name, drive);
}
static int EnableSound(void)
{
    calltrap (6, 2);
}
static int DisableSound(void)
{
    calltrap (6, 1);
}
static int EnableJoystick(void)
{
    calltrap (7, 1);
}
static int DisableJoystick(void)
{
    calltrap (7, 0);
}
static int SetFrameRate(ULONG rate)
{
    calltrap (8, rate);
}
static int ChgCMemSize(ULONG mem)
{
    calltrap (9, mem);
}
static int ChgSMemSize(ULONG mem)
{
    calltrap (10, mem);
}
static int ChgFMemSize(ULONG mem)
{
    calltrap (11, mem);
}
static int ChangeLanguage(ULONG lang)
{
    calltrap (12, lang);
}
static int ExitEmu(void)
{
    calltrap (13);
}
static int GetDisk(ULONG drive, UBYTE *name)
{
    calltrap (14, drive, name);
}
static int DebugFunc(void)
{
    calltrap (15);
}
