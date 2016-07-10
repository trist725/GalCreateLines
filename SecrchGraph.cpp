// SecrchGraph.cpp: implementation of the /*SecrchGraph*/ class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SecrchGraph.h"
#include "MyDlg.h"

extern CMyDlg *g_pMdlsDlg;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSearchGraph::CSearchGraph()
{

}

CSearchGraph::~CSearchGraph()
{

}

void CSearchGraph::Init()
{
	ads_name ss;
	long ss_cnt = 0;
	ads_name entName;
	AcDbObjectId objId;
	AcDbLine *pLine;
	AcDbEntity *pEnt;
	AcGeLineSeg2d Segline;
	AcGePoint3d Pnt3d[2];
	AcGePoint2d Pnt2d[2];
	AcGePoint2d insPt;
	AcGePoint2d pt[4];
	int mFlag = 0;
	int nFlag = 0;
	vector<AcGeLineSeg2d>::iterator m;
	vector<AcGeLineSeg2d>::iterator n;

	AcGeLineSeg2d sl[2];

	resbuf* stResBuf = acutBuildList(RTDXF0, "LINE", 8, "GalLineTest", RTNONE);
	acedSSGet(_T("X"), NULL, NULL, stResBuf, ss);
	acedSSLength(ss, &ss_cnt);

	//获线
	for (long i = 0; i < ss_cnt; ++i)
	{
		acedSSName(ss, i, entName);		//得到选择集SS的第i个
		acdbGetObjectId(objId, entName);	
		acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead);
		pLine = AcDbLine::cast(pEnt);
		Pnt3d[0] = pLine->startPoint();
		Pnt3d[1] = pLine->endPoint();
		Pnt2d[0][X] = Pnt3d[0][X];
		Pnt2d[0][Y] = Pnt3d[0][Y];
		Pnt2d[1][X] = Pnt3d[1][X];
		Pnt2d[1][Y] = Pnt3d[1][Y];
		Segline.set(Pnt2d[0], Pnt2d[1]);
		SeglineVec.push_back(Segline);
	}

	//断线
	for (m = SeglineVec.begin(); m != SeglineVec.end(); ++m)
	{
		for (n = (m+1); n != SeglineVec.end(); ++n)
		{	
			mFlag = 0;
			nFlag = 0;
			if ((*m).intersectWith((*n), insPt) == Adesk::kTrue)
			{
				pt[0] = (*m).startPoint();
				pt[1] = (*m).endPoint();
				pt[2] = (*n).startPoint();
				pt[3] = (*n).endPoint();
				
				for (int s = 0; s < 4; ++s)
				{
					if ((pt[s][X] == insPt[X]) &&
						(pt[s][Y] == insPt[Y]))
					{
						if (s <= 1)		//线段m
						{
							mFlag = 1;	//m不被截断
						}
						else			//线段n
						{
							nFlag = 1;	//n不被截断
						}
					}
				}

				if (!mFlag)
				{
					//删除原线段
					m = SeglineVec.erase(m);					
					sl[0].set(insPt, pt[0]);
					sl[1].set(insPt, pt[1]);	
					//加入新的小线段
					SeglineVec.push_back(sl[0]);
					SeglineVec.push_back(sl[1]);
				}
				if (!nFlag)
				{
					n = SeglineVec.erase(n);
					sl[0].set(insPt, pt[0]);
					sl[1].set(insPt, pt[1]);
					SeglineVec.push_back(sl[0]);
					SeglineVec.push_back(sl[1]);
				}
			}
		}
	}

	//建图
	stPoint stp1;
	stPoint stp2;
	stLine stl;
	long count = 0;
	long a = 0;
	long b = 0;
	bool flag = true;

	stl.isPass = false;
	if (SeglineVec.size() > 0)
	{
		stp1.pnt = SeglineVec[0].startPoint();
		stp2.pnt = SeglineVec[0].endPoint();
		m_Points.push_back(stp1);
		m_Points.push_back(stp2);
		stl.pPnt1 = &(m_Points[0]);
		stl.pPnt2 = &(m_Points[1]);
		stl.psl = &(SeglineVec[0]);
		m_Lines.push_back(stl);
		b++;
		m_Points[0].pstLineVec.push_back(&(m_Lines[0]));
		m_Points[1].pstLineVec.push_back(&(m_Lines[0]));
		a+=2;
	}
	
	
	for (m = SeglineVec.begin(); m != SeglineVec.end(); ++m)
	{	
		flag = true;
		//遍历点
		for (vector<stPoint>::iterator itp = m_Points.begin();
		itp != m_Points.end(); ++itp)
		{
			//若该线2端点已存在点集合中,则该点则指向该线
			if (((*itp).pnt == (*m).startPoint()) ||
				((*itp).pnt == (*m).endPoint()))
			{
				flag = false;
				//通过psl成员找到该线
				for (vector<stLine>::iterator itl = m_Lines.begin();
					itl != m_Lines.end(); ++itl)
				{
						if ((*itl).psl == &(*m))
						{
							(*itp).pstLineVec.push_back(&(*itl));
						}
				}
				break;
			}
		}
		//点未存在则添加点
		if (flag)
		{
			stp1.pnt = (*m).startPoint();
			stp2.pnt = (*m).endPoint();
			m_Points.push_back(stp1);
			m_Points.push_back(stp2);
			stl.pPnt1 = &(m_Points[a]);
			stl.pPnt2 = &(m_Points[a+1]);
			stl.psl = &(SeglineVec[a]);
			m_Lines.push_back(stl);
			b++;
			m_Points[a].pstLineVec.push_back(&(m_Lines[b]));
			m_Points[a+1].pstLineVec.push_back(&(m_Lines[b]));
			a+=2;
		}
	}
}

bool CSearchGraph::SearchPath(const AcGePoint2d& pnt1, const AcGePoint2d& pnt2)
{
	for (vector<stPoint>::iterator it = m_Points.begin();
	it != m_Points.end(); ++it)
	{
		if (pnt1 == (*it).pnt)
		{
			for (vector<stLine*>::iterator it2 = (*it).pstLineVec.begin();
			it2 != (*it).pstLineVec.end(); ++it2)
			{
				if (*((*it2)->pPnt1).pnt == pnt1)
				{
					if (*((*it2)->pPnt2).pnt == pnt2)
					{
						//ok
						return true;
					}
					else
					{
					//递归	SearchPath();
					}
				}
			}
		}
	}
	return false;
}