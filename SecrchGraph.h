// SecrchGraph.h: interface for the SecrchGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECRCHGRAPH_H__9AD5DBC4_AC69_4613_883A_B829735501DD__INCLUDED_)
#define AFX_SECRCHGRAPH_H__9AD5DBC4_AC69_4613_883A_B829735501DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "gelnsg2d.h"
using namespace std;

struct stPoint;

struct stLine
{
	//线段的2端点
	stPoint* pPnt1;
	stPoint* pPnt2;
	bool isPass;
	AcGeLineSeg2d* psl;	//point to SeglineVec
};

struct stPoint
{
	AcGePoint2d pnt;
	
	vector<stLine*> pstLineVec;	//指向经过该点的所有线段的指针
};

class CSearchGraph
{
public:
	void Init();
	bool SearchPath(const AcGePoint2d& pnt1, const AcGePoint2d& pnt2);

	vector<stPoint> m_Points;
	vector<stLine> m_Lines;

//	long m_lPntCnt;
//	long m_lLineCnt;
	vector<AcGeLineSeg2d> SeglineVec;

	CSearchGraph();
	~CSearchGraph();
};

#endif // !defined(AFX_SECRCHGRAPH_H__9AD5DBC4_AC69_4613_883A_B829735501DD__INCLUDED_)
