// Scan.cpp : 实现文件
//

#include "stdafx.h"
#include "Graphics.h"
#include "Scan.h"
#include "afxdialogex.h"




// CScan 对话框

IMPLEMENT_DYNAMIC(CScan, CDialogEx)

CScan::CScan(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScan::IDD, pParent)
	, seedx(0)
	, seedy(0)
	, red(0)
	, green(0)
	, blue(255)
{
	p[0].x=120;p[0].y=-50;
	p[1].x=0;p[1].y=150;
	p[2].x=-100;p[2].y=100;
	p[3].x=-150;p[3].y=50;
	p[4].x=-190;p[4].y=-150;
	p[5].x=-100;p[5].y=-100;
	p[6].x=100;p[6].y=-170;
	p[7].x=30;p[7].y=0;
	n=0;
	haveCre=0;

}

CScan::~CScan()
{
}

void CScan::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, edges);
	DDX_Text(pDX, IDC_EDIT1, seedx);
	DDX_Text(pDX, IDC_EDIT2, seedy);
	DDX_Text(pDX, IDC_EDIT3, red);
	DDX_Text(pDX, IDC_EDIT4, green);
	DDX_Text(pDX, IDC_EDIT5, blue);
}


BEGIN_MESSAGE_MAP(CScan, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CScan::OnBnClickedButton1)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON2, &CScan::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CScan::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CScan::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CScan::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT3, &CScan::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT5, &CScan::OnEnChangeEdit5)
END_MESSAGE_MAP()


// CScan 消息处理程序


void CScan::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if(n==0)
	{
		AfxMessageBox(_T("请选择多边形边数！"));
		return;
	}
	if(haveCre==1)
	{
		AfxMessageBox(_T("先清屏再重新生成多边形！"));
		return;
	}
	haveCre=1;
	CClientDC dc(this);
	ChangeOrg(dc);
	DrawPolyLine(dc,n,p,RGB(0,0,0));
	
}
void CScan::DrawPolyLine(CDC &dc,int n,CPoint* p,COLORREF color)
{
	
	CPen myPen;
	myPen.CreatePen(PS_SOLID, 2, color);
	SelectObject(dc,myPen);
	Polyline(dc,p,n);
	MoveToEx(dc,p[0].x,p[0].y,NULL);
	LineTo(dc,p[n-1].x,p[n-1].y);
	DeleteObject(myPen);

}
/*边界标志法扫描转换*/
void CScan::Fill_EdgeMark(CDC& dc,int n,CPoint* p,COLORREF edgeClr,COLORREF fillClr)
{
	if(n==2||n==1)
		return;
	/*消去原有的多边形*/
	DrawPolyLine(dc,n,p,RGB(255,255,255));
	/*使用改变过的DDA算法绘制新的多边形，使直线每个y上只有一个像素*/
	for(int i=0;i<n;i++)
	{
		if(i!=n-1)
		drawLineDDA_dy(dc,p[i].x, p[i].y, p[i+1].x,p[i+1].y,edgeClr) ; 
		else
			drawLineDDA_dy(dc,p[i].x, p[i].y, p[0].x,p[0].y,edgeClr) ; 
	}
	
  /*求出多边形两个方向的范围*/
		int xmax,xmin,ymax,ymin;
	xmax=xmin=p[0].x;
	ymax=ymin=p[0].y;
	for(int i=0;i<n;i++)
	{
		if(p[i].x>xmax)
			xmax=p[i].x;
		if(p[i].x<xmin)
			xmin=p[i].x;
		if(p[i].y>ymax)
			ymax=p[i].y;
		if(p[i].y<ymin)
			ymin=p[i].y;
	}
	int sum=0;
	int checky=0;
	int checkx=0;
	/*将多边形顶点填充白色，去除顶点影响*/
	for(int i=0;i<n;i++)
	{
		int t1,t2;
		t1=i-1;t2=i+1;
		if(i==0)
		{
			t1=n-1;	
		}
		if(i==n-1)
		{
			t2=0;
		}
	if(p[t1].y>p[i].y&&p[t2].y>p[i].y)
	{
		SetPixel(dc,p[i].x,p[i].y,RGB(255,255,255));
		
	
	}
		
	if(p[t2].y<p[i].y&&p[t1].y<p[i].y)
	{
		SetPixel(dc,p[i].x,p[i].y,RGB(255,255,255));
		
	}
	}

	
	int flag=-1;
	COLORREF clr;
	/*循环中，遇到边缘就打开开关开始填充，再次遇到，就关闭开关停止填充*/
	for(int i=ymin;i<=ymax;i++)
	{
		flag=-1;
		for(int j=xmin;j<=xmax;j++)
		{
			clr=GetPixel(dc,j,i);
			if(flag==1)
			{
				SetPixel(dc,j,i,fillClr);
			}   
			if(clr==edgeClr)
			{
				
				flag=-flag;
			}

		}
  }
	/*重新用正常的直线绘制多边形*/
	for(int i=0;i<n;i++)
	{
		SetPixel(dc,p[i].x,p[i].y,edgeClr);
	}
	DrawPolyLine(dc,n,p,edgeClr);
	
}




void CScan::drawLineDDA_dy(CDC &dc,int x0, int y0, int xEnd, int yEnd,COLORREF color)  
{  
	if(y0==yEnd)
	{
		SetPixel(dc,x0,y0,color);
		SetPixel(dc,xEnd,yEnd,color);
	}
	else
	{
		int dx=xEnd-x0,dy=yEnd-y0,steps,k;  
		double xIncrement,yIncrement,x=x0,y=y0;  

		steps=abs(dy);
		xIncrement=double(dx)/double(steps);  
		yIncrement=double(dy)/double(steps);  
		SetPixel(dc,x0,y0,color);
		for(k=0;k<steps;k++)
		{  
			x+=xIncrement;  
			y+=yIncrement;  

			SetPixel(dc,(int)(x+0.5),y,color);
		} 
	}
}  

void CScan::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
}


void CScan::OnBnClickedButton2()
{
	if(haveCre==0)
	{
		AfxMessageBox(_T("请先选择生成多边形！"));
		return;
	}
	// TODO: 在此添加控件通知处理程序代码
	if(n==0)
	{
		AfxMessageBox(_T("请选择多边形边数！"));
		return;
	}
	CClientDC dc(this);
	ChangeOrg(dc);
	UpdateData(TRUE);
	Fill_EdgeMark(dc,n,p,RGB(0,0,0),RGB(red,green,blue));
}


	/*扫描线种子填充算法*/
void CScan::SeedFill_Edge(CDC &dc,int x0,int y0,COLORREF fillClr,COLORREF edgeClr)
{
	int count=0;
	int xRight,xLeft;
		/*设立顶点栈*/
	std::stack<CPoint> stk;
	//种子入栈
	stk.push(CPoint(x0,y0));
	//栈空时，种子上下行全部填充，填充完成
	while(!stk.empty())
	{
		//每次取第一个点做种子
		CPoint seed = stk.top();
		stk.pop();
		//将种子右边界填充并返回填充的数目
		count = FillLineRight(dc,seed.x,seed.y,edgeClr,fillClr);
		if(count==-1) return;
		//计算右边界坐标
		 xRight = seed.x + count - 1;
		 //将种子左边界填充并返回填充的数目
		  count = FillLineLeft(dc,seed.x,seed.y,edgeClr,fillClr);
		  if(count==-1) return;
		  //计算左边界坐标
		  xLeft = seed.x - count;
		  //对于上下的两条像素，分别查找在左右边界内，是否填充
		  //若有未填充的值，则将其最右端的未填充像素入栈
		  SearchNewSeed(dc,stk, xLeft, xRight, seed.y - 1, edgeClr,fillClr);
		  SearchNewSeed(dc,stk, xLeft, xRight, seed.y + 1, edgeClr,fillClr);
	}
}

int CScan::FillLineRight(CDC &dc,int x, int y, COLORREF edgeClr,COLORREF fillClr)
{
	int count=0;
	
	while(GetPixel(dc,x+count,y)!=edgeClr)
	{
		SetPixel(dc,x+count,y,fillClr);
		count++;
		if(count>2000)
		{
			AfxMessageBox(_T("选取的点不在图形内部！"));
			return -1;
		}
	}
	return count;
}
int CScan::FillLineLeft(CDC &dc,int x, int y, COLORREF edgeClr,COLORREF fillClr)
{
	int count=0;
	while(GetPixel(dc,x-count,y)!=edgeClr)
	{
		SetPixel(dc,x-count,y,fillClr);
		count++;
		if(count>2000)
		{
			AfxMessageBox(_T("选取的点不在图形内部！"));
			return -1;
		}
	}
	return count;
}
//寻找一行像素中的新的种子
 void CScan::SearchNewSeed(CDC &dc,std::stack<CPoint>& stk, int xLeft, int xRight, int y, COLORREF edgeClr,COLORREF fillClr)
 {
	  int xt = xLeft;
	  bool findNewSeed = false;
	  while(xt <= xRight)
	  {
		   findNewSeed = false;
		   //找到未被填充的像素，开关置真，新的种子加一
		   while(GetPixel(dc,xt,y)!=fillClr&&GetPixel(dc,xt,y)!=edgeClr&&xt<=xRight)
		   {
			   findNewSeed=TRUE;
			   xt++;
		   }
		   
		   if(findNewSeed)
		   {//将新的种子入栈，若种子刚好进入边缘，则减一后入栈
			    if( GetPixel(dc,xt,y)!=fillClr&&GetPixel(dc,xt,y)!=edgeClr&& (xt == xRight))
					stk.push(CPoint(xt, y));
				else
					stk.push(CPoint(xt-1, y));
		   }
		   //碰到边缘或者已经填充的像素，就跳过，种子不入栈
		   //知道找到新的未填充像素
		   xt+=1;
	  }
 }

 void CScan::OnBnClickedButton3()
 {
	 if(haveCre==0)
	 {
		 AfxMessageBox(_T("请先选择生成多边形！"));
		 return;
	 }
	 if(n==0)
	 {
		 AfxMessageBox(_T("请选择多边形边数！"));
		 return;
	 }
	 // TODO: 在此添加控件通知处理程序代码
	 CClientDC dc(this);
	 ChangeOrg(dc);
	 UpdateData(TRUE);
	 SeedFill_Edge(dc,seedx,seedy,RGB(red,green,blue),RGB(0,0,0));
		 	
 }


 void CScan::OnBnClickedButton4()
 {
	 // TODO: 在此添加控件通知处理程序代码
	 CRect r;
	 GetClientRect(r);
	 InvalidateRect(&r,TRUE);
	 haveCre=0;
 }

 void CScan::ChangeOrg(CDC& dc)
 {

	 SetMapMode(dc,MM_ANISOTROPIC); 
	 CRect rect;
	 GetClientRect(&rect);
	 SetViewportOrgEx(dc,rect.right/2, rect.bottom/2,NULL);
	 SetViewportExtEx(dc,rect.right,rect.bottom,NULL);
	 SetWindowOrgEx(dc,0,0,NULL);
	 SetWindowExtEx(dc,rect.right,rect.bottom*-1,NULL);
 }

 BOOL CScan::OnInitDialog()
 {
	 CDialogEx::OnInitDialog();

	 // TODO:  在此添加额外的初始化
	 edges.AddString(_T("3"));
	 edges.AddString(_T("4"));
	 edges.AddString(_T("5"));
	 edges.AddString(_T("6"));
	 edges.AddString(_T("7"));
	 

	 MoveWindow(0,0,1000,600,TRUE);
	 CenterWindow();
	 return TRUE;  // return TRUE unless you set the focus to a control
	 // 异常: OCX 属性页应返回 FALSE
 }


 void CScan::OnCbnSelchangeCombo1()
 {
	 // TODO: 在此添加控件通知处理程序代码
	 CString msg;
	 edges.GetLBText(edges.GetCurSel(),msg);
	 n=_ttoi(msg);		 
 }


 void CScan::OnEnChangeEdit3()
 {
	 // TODO:  如果该控件是 RICHEDIT 控件，它将不
	 // 发送此通知，除非重写 CDialogEx::OnInitDialog()
	 // 函数并调用 CRichEditCtrl().SetEventMask()，
	 // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	 // TODO:  在此添加控件通知处理程序代码
 }


 void CScan::OnEnChangeEdit5()
 {
	 // TODO:  如果该控件是 RICHEDIT 控件，它将不
	 // 发送此通知，除非重写 CDialogEx::OnInitDialog()
	 // 函数并调用 CRichEditCtrl().SetEventMask()，
	 // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	 // TODO:  在此添加控件通知处理程序代码
 }
