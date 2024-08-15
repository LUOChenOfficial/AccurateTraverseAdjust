#pragma once

//枚举数据类型，用于代表角度形式
enum AngleStyle
{
	DEG,
	DMS,
	RAD
};
class CAngle
{
public:
	CAngle(double value = 0, AngleStyle style = DMS);
	~CAngle(void);
private:
	double dValue;//角度值
	AngleStyle  nCurStyle;//当前角度值类型
private:
	//设置常成员函数的作用：1.类成员不会被改变
	//2.可以被常类变量调用
	double Deg(double dDms) const;
	double Dms(double dDeg) const;

public:
	//获取指定的类型获取角度值，
	//由于返回的是dValue的引用，所以该值大小可以改变，即可以进行赋值
	double& operator() (AngleStyle style);   //CAanle ang1,   ang1.operator(DEG); angl1(DEG)

	//重载，获取指定的类型获取角度值，该值不可改变，const CAngle类型变量调用
	double operator() (AngleStyle style) const;
	//重载运算符+/-
	friend CAngle operator + (const CAngle& m1, const CAngle& m2);
	friend CAngle operator - (const CAngle& m1, const CAngle& m2);
};


