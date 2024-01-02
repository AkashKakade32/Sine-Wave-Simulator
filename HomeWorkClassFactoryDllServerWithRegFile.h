#pragma once

class ICircle :public IUnknown
{
public:
	virtual HRESULT __stdcall PointOnCircle(double, double, int*, int*) = 0;
};

class ISineWave : public IUnknown
{
public:
	virtual HRESULT __stdcall PointOnSineWave(double, double, double,int*, int*) = 0;
};

//CLSID of SumofSubtract Component {D5C2F241-C4F4-40C1-A818-13F5822E5F54}
const CLSID CLSID_SumSubtract = { 0xd5c2f241, 0xc4f4, 0x40c1, 0xa8, 0x18, 0x13, 0xf5, 0x82, 0x2e, 0x5f, 0x54 };

//IID of ICircle Interface {9EA24CA0-1440-4E5D-9222-36B21E3520B8}
const IID IID_ICircle = { 0x9ea24ca0, 0x1440, 0x4e5d, 0x92, 0x22, 0x36, 0xb2, 0x1e, 0x35, 0x20, 0xb8 };


//IID of ISineWave Interface {8651F6C2-BC7A-43AA-8B91-2F8AE6B0BE30}
const IID IID_ISineWave = { 0x8651f6c2, 0xbc7a, 0x43aa, 0x8b, 0x91, 0x2f, 0x8a, 0xe6, 0xb0, 0xbe, 0x30 };