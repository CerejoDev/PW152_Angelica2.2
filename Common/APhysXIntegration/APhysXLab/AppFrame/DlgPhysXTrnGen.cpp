// DlgPhysXTrnGen.cpp : implementation file
//

#include "stdafx.h"
#include "APhysXLab.h"
#include "DlgPhysXTrnGen.h"

#include "APhysX.h"

#include <CommonFileName.h>
#include <A3DMacros.h>
#include <A3DTerrain2.h>
#include <A3DTerrain2LOD.h>
#include <A3DTerrain2Blk.h>
#include <A3DTerrain2Loader.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPhysXTrnGen dialog


CDlgPhysXTrnGen::CDlgPhysXTrnGen(const char* pCurrentTrnName, A3DEngine& eng, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPhysXTrnGen::IDD, pParent), m_A3DEngine(eng)
{
	//{{AFX_DATA_INIT(CDlgPhysXTrnGen)
	m_strTrnFile = _T("");
	//}}AFX_DATA_INIT

	m_pTerrain	 = 0;
	m_pHeightMap = 0;
	m_CurTrnFilename = pCurrentTrnName;
}


void CDlgPhysXTrnGen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPhysXTrnGen)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_EDIT_TRNINFO, m_TrnInfoEdit);
	DDX_Text(pDX, IDC_EDIT_TRNFILE, m_strTrnFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPhysXTrnGen, CDialog)
	//{{AFX_MSG_MAP(CDlgPhysXTrnGen)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPhysXTrnGen message handlers

BOOL CDlgPhysXTrnGen::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPhysXTrnGen::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	A3DRELEASE(m_pTerrain);
}

void CDlgPhysXTrnGen::OnOK() 
{
	// TODO: Add extra validation here
	if (!UpdateData(TRUE))
		return;
	
	if (!m_pTerrain)
	{
		MessageBox(_T("请先选择一个地图数据源！"));
		return;
	}
	
	if (BuildTerrainPhysXData())
	{
		MessageBox(_T("成功！"));
		CDialog::OnOK();
		return;
	}

	MessageBox(_T("失败！"));
}

void CDlgPhysXTrnGen::OnBtnBrowse() 
{
	// TODO: Add your control notification handler code here
	CCommonFileName FileDlg(&m_A3DEngine, 0);
	FileDlg.SetFileExtension("trn2");
	if (FileDlg.DoModal() != IDOK)
		return;
	
	AString strFile = FileDlg.GetFullFileName();
	
	//	Try to open terrain file and read info from it
	char szFile[MAX_PATH];
	af_GetRelativePath(m_CurTrnFilename, szFile);
	if (!strFile.CompareNoCase(szFile))
	{
		MessageBox(_T("不能选择当前正在游览的地形"));
		return;
	}
	
	if (!LoadTerrain(strFile))
	{
		MessageBox(_T("地形数据加载失败！"));
		return;
	}

	PrintTerrainInfo();

	m_strTrnFile = strFile;
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_TRNFILE, m_strTrnFile);
}

bool CDlgPhysXTrnGen::LoadTerrain(const char* szFile)
{
	A3DTerrain2* pNewTerrain = new A3DTerrain2;
	if (!pNewTerrain->Init(&m_A3DEngine, 512.0f, 512.0f))
	{
		delete pNewTerrain;
		a_LogOutput(1, "CDlgPhysXTrnGen::LoadTerrain: Failed to initialize A3DTerrain2!");
		return false;
	}
	
	if (!pNewTerrain->Load(szFile, 0.0f, 0.0f))
	{
		A3DRELEASE(pNewTerrain);
		a_LogOutput(1, "CDlgPhysXTrnGen::LoadTerrain: Failed to load A3DTerrain2!");
		return false;
	}
	
	A3DRELEASE(m_pTerrain);
	m_pTerrain = pNewTerrain;
	
	//	Reset terrain's loading radius
	float fRadius = m_pTerrain->GetSubTerrainGrid() * m_pTerrain->GetGridSize() * 0.5f;
	m_pTerrain->SetActRadius(fRadius);
	m_pTerrain->SetViewRadius(fRadius);
	return true;
}

void CDlgPhysXTrnGen::PrintTerrainInfo()
{
	if (0 == m_pTerrain)
		return;

	//	Claer edit box
	m_TrnInfoEdit.SetSel(0, -1);
	m_TrnInfoEdit.ReplaceSel(_T(""));
		
	char szText[200];
	
	//	Loader type
	if (m_pTerrain->GetTerrainLoader()->GetLoaderID() == A3DTerrain2Loader::LOADER_A)
		sprintf(szText, "Loader type:    LOADER_A\n");
	else
		sprintf(szText, "Loader type:    LOADER_B\n");
	
	InsertInfoText(szText);
	
	//	sub-terrain number
	if (m_pTerrain->GetTerrainLoader()->GetLoaderID() == A3DTerrain2Loader::LOADER_A)
	{
		sprintf(szText, "Sub-terrain number:    1\n");
	}
	else if (m_pTerrain->GetTerrainLoader()->GetLoaderID() == A3DTerrain2Loader::LOADER_B)
	{
		int r = m_pTerrain->GetBlockRowNum() * m_pTerrain->GetBlockGrid() / m_pTerrain->GetSubTerrainGrid();
		int c = m_pTerrain->GetBlockColNum() * m_pTerrain->GetBlockGrid() / m_pTerrain->GetSubTerrainGrid();
		sprintf(szText, "Sub-terrain number:    C * R = %d * %d = %d\n", c, r, r * c);
		InsertInfoText(szText);
	}
	
	//	terrain size
	const ARectF& rc = m_pTerrain->GetTerrainArea();
	sprintf(szText, "Terrain size:    W * L = %.2f * %.2f\n", rc.Width(), -rc.Height());
	InsertInfoText(szText);
	
	//	grid size
	sprintf(szText, "Grid size:    %.2f\n", m_pTerrain->GetGridSize());
	InsertInfoText(szText);
	
	//	block grid
	sprintf(szText, "Grid of each block:    %d\n", m_pTerrain->GetBlockGrid());
	InsertInfoText(szText);
	
	//	block size
	sprintf(szText, "Block size:    %.2f\n", m_pTerrain->GetBlockGrid() * m_pTerrain->GetGridSize());
	InsertInfoText(szText);
	
	//	mask grid
	sprintf(szText, "Grid of each mask area:    %d\n", m_pTerrain->GetMaskGrid());
	InsertInfoText(szText);
	
	//	mask size
	sprintf(szText, "Mask area size:    %.2f\n", m_pTerrain->GetMaskGrid() * m_pTerrain->GetGridSize());
	InsertInfoText(szText);
	
	//	sub-terrain grid
	sprintf(szText, "Grid of each sub-terrain:    %d\n", m_pTerrain->GetSubTerrainGrid());
	InsertInfoText(szText);
	
	//	sub_terrain size
	sprintf(szText, "Sub-terrain size:    %.2f\n", m_pTerrain->GetSubTerrainGrid() * m_pTerrain->GetGridSize());
	InsertInfoText(szText);
}

void CDlgPhysXTrnGen::InsertInfoText(const char* szText)
{
	//	Select all text at first then to cancel selection. This is the way to
	//	ensure caret is at the end of text when new text is added !!
	m_TrnInfoEdit.SetSel(0, -1);
	m_TrnInfoEdit.SetSel(-1, -1);
	m_TrnInfoEdit.ReplaceSel(_TAS2WC(szText));
}

bool CDlgPhysXTrnGen::BuildTerrainPhysXData()
{
	if (0 == m_pTerrain)
		return false;
	
	int iSubTrnGrid = m_pTerrain->GetSubTerrainGrid();
	float fSubTrnSize = iSubTrnGrid * m_pTerrain->GetGridSize();
	
	//	Create height map buffer
	int iNumVert = (iSubTrnGrid+1) * (iSubTrnGrid+1);
	m_pHeightMap = new float [iNumVert];
	if (!m_pHeightMap)
		return false;

	//----------------------------------------------------------------------------------
	// create extra cell data and patch data...
	m_pAPhysXTBData = new APhysXTerrainBlockData;

	m_pAPhysXTBData->mExtraCellData = new APhysXU32[iSubTrnGrid * iSubTrnGrid];

	int iPatchNum = m_pTerrain->GetSubTerrainGrid() / m_pTerrain->GetBlockGrid();
	m_pAPhysXTBData->mExtraPatchData = new APhysXU32[iPatchNum * iPatchNum];
	//----------------------------------------------------------------------------------

	
	//	Reset progress control
	m_Progress.SetRange(0, m_pTerrain->GetBlockNum());
	m_Progress.SetStep(1);
	m_Progress.SetPos(0);
	
	int r, c, iSubIndex = 0;
	int iNumRow = m_pTerrain->GetBlockRowNum() * m_pTerrain->GetBlockGrid() / m_pTerrain->GetSubTerrainGrid();
	int iNumCol = m_pTerrain->GetBlockColNum() * m_pTerrain->GetBlockGrid() / m_pTerrain->GetSubTerrainGrid();
	
	const ARectF& rcTerrain = m_pTerrain->GetTerrainArea();
	
	for (r=0; r < iNumRow; r++)
	{
		for (c=0; c < iNumCol; c++)
		{
			//	Calculate sub-terrain's center and use it as the loading center for terrain
			A3DVECTOR3 vCenter;
			vCenter.x = rcTerrain.left + (c+1) * fSubTrnSize - fSubTrnSize * 0.5f;
			vCenter.z = rcTerrain.top - (r+1) * fSubTrnSize + fSubTrnSize * 0.5f;
			vCenter.y = 0.0f;
			m_pTerrain->SetLoadCenter(vCenter);
			
			//	Build block squares in current active area
			BuildSubTerrainPhysXData(r, c, iSubIndex);
			iSubIndex++;
		}
	}
	
	//	Release height map buffer
	delete [] m_pHeightMap;
	m_pHeightMap = NULL;

	//----------------------------------------------------------------------------------
	// release extra cell data and patch data...
	
	delete [] m_pAPhysXTBData->mExtraCellData;
	m_pAPhysXTBData->mExtraCellData = NULL;

	delete [] m_pAPhysXTBData->mExtraPatchData;
	m_pAPhysXTBData->mExtraPatchData = NULL;

	delete m_pAPhysXTBData;
	m_pAPhysXTBData = NULL;
	//----------------------------------------------------------------------------------

	//	Generate physX terrain config and save it to file
	if (!SavePhysXDataConfigs())
	{
		a_LogOutput(1, "CDlgPhysXTrnGen::BuildTerrainPhysXData", "Failed to call SavePhysXDataConfigs()");
		return false;
	}
	
	return true;
}

bool CDlgPhysXTrnGen::BuildSubTerrainPhysXData(int r, int c, int iSubIndex)
{
	int iSubTrnGrid = m_pTerrain->GetSubTerrainGrid();
	int iSubTrnBlock = iSubTrnGrid / m_pTerrain->GetBlockGrid();
	int iSubTrnSize = iSubTrnGrid + 1;
	
	int i, j;
	
	//	Clear height map buffer
	int iNumVert = iSubTrnSize * iSubTrnSize;
	for (i=0; i < iNumVert; i++)
		m_pHeightMap[i] = -1.0f;
	
	//-----------------------------------------------------------------
	// clear the extra cell and patch data buffer...
	int iCellNum = iSubTrnGrid * iSubTrnGrid;
	for (i=0; i < iCellNum; i++)
		m_pAPhysXTBData->mExtraCellData[i] = 0;

	int iPatchNum = iSubTrnBlock * iSubTrnBlock;
	for (i=0; i < iPatchNum; i++)
		m_pAPhysXTBData->mExtraPatchData[i] = 0;

	
	m_pAPhysXTBData->mRowNum = m_pAPhysXTBData->mColumnNum = iSubTrnGrid;
	m_pAPhysXTBData->mPatchRowNum = m_pAPhysXTBData->mPatchColumnNum = m_pTerrain->GetBlockGrid();
	
	//-----------------------------------------------------------------
	
	ARectI rc;
	rc.left		= iSubTrnBlock * c;
	rc.right	= rc.left + iSubTrnBlock;
	rc.top		= iSubTrnBlock * r;
	rc.bottom	= rc.top + iSubTrnBlock;
	
	A3DTerrain2::ACTBLOCKS* pActArea = m_pTerrain->GetActiveBlocks();
	
	for (i=rc.top; i < rc.bottom; i++)
	{
		for (j=rc.left; j < rc.right; j++)
		{
			A3DTerrain2Block* pBlock = pActArea->GetBlock(i, j, false);
			ASSERT(pBlock);
			
			int br = i - rc.top;
			int bc = j - rc.left;
			FillBlockHeight(pBlock, br, bc);

			//-------------------------------------------------------------
			// test code: to make patch[1,1] as a big hole
			/*
			if(br == 1 && bc == 2)
			{
				m_pAPhysXTBData->SetExtraPatchData(br, bc, APX_TP_EXTRADATA_HOLE);
			}
			*/
			//-------------------------------------------------------------
			
			m_Progress.StepIt();
		}
	}
	
	//	Find the minimum height and maximum height of height map
	float fMinHei=FLT_MAX, fMaxHei=FLT_MIN;
	
	for (i=0; i < iNumVert; i++)
	{
		float f = m_pHeightMap[i];
		if (f < fMinHei) fMinHei = f;
		if (f > fMaxHei) fMaxHei = f;
	}
	
	//	Generate physX terrain data from height map and save it to disk
	if (!SaveSubTerrainPhysXData(iSubIndex, fMinHei, fMaxHei))
	{
		a_LogOutput(1, "CDlgPhysXTrnGen::BuildSubTerrainPhysXData", "Failed to call SaveSubTerrainPhysXData() for sub-terrain (%d)", iSubIndex);
		return false;
	}
	
	return true;
}

void CDlgPhysXTrnGen::FillBlockHeight(A3DTerrain2Block* pBlock, int br, int bc)
{
	ASSERT(pBlock);
	
	//	Note: vertices in block are not ordered from top to bottom, from 
	//		left to right. so we have to re-map vertex index
	WORD* aIndexMaps = m_pTerrain->GetLODManager()->GetIndexMaps();
	int iSubTrnPitch = m_pTerrain->GetSubTerrainGrid() + 1;
	const int iSize = pBlock->GetBlockGrid() + 1;
	
	int i, j;
	float* pDstStart = m_pHeightMap + br * pBlock->GetBlockGrid() * iSubTrnPitch + bc * pBlock->GetBlockGrid();
	
	for (i=0; i < iSize; i++)
	{
		int iVertIdx = i * iSize;
		float* pDst = pDstStart;
		pDstStart += iSubTrnPitch;
		
		for (j=0; j < iSize; j++, iVertIdx++, pDst++)
		{
			int n = aIndexMaps[iVertIdx];
			*pDst = pBlock->GetVertexPos(n).y;
		}
	}

	//-------------------------------------------------------------
	// test code: to make some cells as a hole
	/*
	for (i=10; i < 30; i++)
	{
		for (j=100; j < 110; j++)
			m_pAPhysXTBData->SetExtraCellData(i, j, APX_TC_EXTRADATA_HOLE);

	}
	*/
	//-------------------------------------------------------------

}

bool CDlgPhysXTrnGen::SavePhysXDataConfigs()
{
	int iSubTrnGrid = m_pTerrain->GetSubTerrainGrid();
	
	APhysXTerrainConfigs cfg;
	cfg.mRowNumOfBlocks			= m_pTerrain->GetBlockRowNum() * m_pTerrain->GetBlockGrid() / iSubTrnGrid;
	cfg.mColNumOfBlocks			= m_pTerrain->GetBlockColNum() * m_pTerrain->GetBlockGrid() / iSubTrnGrid;
	cfg.mBlockSizeX				= iSubTrnGrid * m_pTerrain->GetGridSize();
	cfg.mBlockSizeZ				= iSubTrnGrid * m_pTerrain->GetGridSize();
	cfg.mTerrainGlobalOrigin	= APX_TerrainGlobalOrigin_Center;
	
	AString strPrefix;
	af_GetFileTitle(_TWC2AS(m_strTrnFile), strPrefix);
	af_ChangeFileExt(strPrefix, "");
	cfg.mTerrainBlockFilePrefix	= strPrefix;
	
	//	Generate config file name
	AString strTitle = _TWC2AS(m_strTrnFile);
	af_ChangeFileExt(strTitle, "_ptb.cfg");
	AString strFile = af_GetBaseDir();
	strFile += "\\" + strTitle;
	
	//	Save config to file
	APhysXTerrain trn;
	trn.LoadConfigs(cfg);
	trn.SaveConfigs(strFile);
	
	return true;
}

bool CDlgPhysXTrnGen::SaveSubTerrainPhysXData(int iSubIndex, float fMinHei, float fMaxHei)
{
	int iSubTrnGrid = m_pTerrain->GetSubTerrainGrid();
	
	APhysXTerrainBlockData data;
	data.mColumnNum			= iSubTrnGrid;
	data.mRowNum			= iSubTrnGrid;
	data.mPatchColumnNum	= m_pTerrain->GetBlockGrid();
	data.mPatchRowNum		= m_pTerrain->GetBlockGrid();
	data.mSampleData		= m_pHeightMap;
	data.mMinHeight			= fMinHei;
	data.mMaxHeight			= fMaxHei;
	data.mSizeX				= iSubTrnGrid * m_pTerrain->GetGridSize();
	data.mSizeZ				= iSubTrnGrid * m_pTerrain->GetGridSize();
	
	//--------------------------------------------------------------------
	data.mExtraCellData = m_pAPhysXTBData->mExtraCellData;
	data.mExtraPatchData = m_pAPhysXTBData->mExtraPatchData;

	//	Generate file name
	AString strPath, strTitle;
	af_GetFilePath(_TWC2AS(m_strTrnFile), strPath);
	af_GetFileTitle(_TWC2AS(m_strTrnFile), strTitle);
	
	strPath = "\\" + strPath + "\\";
	strPath = af_GetBaseDir() + strPath;
	
	char szExt[32];
	sprintf(szExt, "_%d.ptb", iSubIndex+1);
	af_ChangeFileExt(strTitle, szExt);
	
	//	Save data to file
	APhysXTerrainBlock Block;
	Block.ImportTerrainData(data);
	
	if (!Block.Save(strPath + strTitle))
		return false;
	
	return true;
}
