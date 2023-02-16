// DialogModel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PMInspect.h"
#include "DialogModel.h"
#include "afxdialogex.h"

#include "PMInspectDlg.h"

// CDialogModel 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogModel, CDialogEx)

CDialogModel::CDialogModel(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MODEL, pParent)
	, m_edit_strName(_T(""))
	, m_edit_strDescription(_T(""))
{
	m_iModelIdx = -1;

	//1107
	memset(m_szMainPath, NULL, sizeof(m_szMainPath));
	sprintf_s(m_szMainPath, sizeof(m_szMainPath), MOD_MODEL_PATH);

	memset(m_szMainRegi, NULL, sizeof(m_szMainRegi));
	sprintf_s(m_szMainRegi, sizeof(m_szMainRegi), MOD_MODEL_REGI);

	memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
}

CDialogModel::~CDialogModel()
{
}

void CDialogModel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MODEL_NAME, m_edit_strName);
	DDX_Text(pDX, IDC_EDIT_MODEL_DESCRIPTION, m_edit_strDescription);
	DDX_Control(pDX, IDC_LIST_MODEL, m_listctrl_model);
}


BEGIN_MESSAGE_MAP(CDialogModel, CDialogEx)
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MODEL, &CDialogModel::OnNMDblclkListModel)
END_MESSAGE_MAP()


// CDialogModel 메시지 처리기입니다.
BOOL CDialogModel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	int i;
	long setstyle;
	LV_COLUMN column;
	CImageList *img_list_slct;
	CBitmap bitmap;

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	img_list_slct = new CImageList;
	img_list_slct->Create(16, 16, ILC_COLOR4, 3, 3);

	bitmap.LoadBitmap(IDB_BITMAP_SELECTED);
	img_list_slct->Add(&bitmap, RGB(0, 0, 0));
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP_NOTSELECTED);
	img_list_slct->Add(&bitmap, RGB(0, 0, 0));
	bitmap.DeleteObject();
	m_listctrl_model.SetImageList(img_list_slct, LVSIL_SMALL);

	setstyle = GetWindowLong(m_listctrl_model.m_hWnd, GWL_STYLE);
	setstyle |= LVS_EDITLABELS;
	SetWindowLong(m_listctrl_model.m_hWnd, GWL_STYLE, setstyle);

	m_listctrl_model.RedrawWindow();


	TCHAR* list[4] = { _T("제품명"), _T("설명"), _T("경로명") };
	int width[4] = { 500, 300, 200 };
	for (i = 0; i<3; i++)
	{
		column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		column.fmt = LVCFMT_LEFT;
		column.pszText = list[i];
		column.iSubItem = i;
		column.cx = width[i];
		m_listctrl_model.InsertColumn(i, &column);
	}
	m_listctrl_model.DeleteAllItems();

	//데이타 불러오기////////////////////////////
	CString str0, str1, path;

	typeModel mod;
	//memset(&mod, NULL, sizeof(mod));

	BOOL name_not_null = TRUE;
	BOOL name_not_dupl = TRUE;
	int list_cnt, dupl_check;

	path.Empty();
	path.Format(_T("%S\\NAME"), m_szMainRegi);

	//모든데이타 불러오기
	int idx = 0;
	while (1)
	{
		str0.Empty();
		str0.Format(_T("%d"), idx);

		memset(&mod, NULL, sizeof(mod));

		//제품명
		path.Empty();
		path.Format(_T("%S\\NAME"), m_szMainRegi);
		if (!GetRegString(path, str0, (LPBYTE)mod.szName))
		{
			break;
		}

		//설명
		path.Empty();
		path.Format(_T("%S\\DESCRIPTION"), m_szMainRegi);
		if (!GetRegString(path, str0, (LPBYTE)mod.szDesc))
		{
			break;
		}

		//경로명
		path.Empty();
		path.Format(_T("%S\\PATH"), m_szMainRegi);
		if (!GetRegString(path, str0, (LPBYTE)mod.szPath))
		{
			break;
		}

		//Null 및 중복 확인
		str0.Empty();
		str0 = mod.szName;
		str1.Empty();
		str1 = mod.szPath;
		if (!(str0.IsEmpty() || str1.IsEmpty()))
		{
			name_not_dupl = TRUE;
			//리스트에 같은 이름이 존재하는지 확인
			dupl_check = 0;
			list_cnt = 0;
			list_cnt = m_listctrl_model.GetItemCount();
			for (i = 0; i < list_cnt; i++)
			{
				str1.Empty();
				str1 = m_listctrl_model.GetItemText(i, 0);
				dupl_check = str0.Compare(str1);
				if (0 == dupl_check)
				{
					name_not_dupl = FALSE;
					break;
				}
			}
			if (name_not_dupl)
			{
				//리스트 구성
				AddList(&mod);
			}
		}

		idx++;
	}

	//20130201
	path.Empty();
	path.Format(_T("%S\\FULLPATH"), m_szMainRegi);
	str0 = _T("0");
	GetRegString(path, str0, (LPBYTE)m_szMainPath);


	//현재생산데이타 불러오기
	memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));


	//제품명
	path.Empty();
	path.Format(_T("%S\\CURRENT"), m_szMainRegi);
	if (GetRegString(path, _T("NAME"), (LPBYTE)m_CurrentModel.szName))
	{
		//설명
		if (!GetRegString(path, _T("DESCRIPTION"), (LPBYTE)m_CurrentModel.szDesc))
		{
			memset(m_CurrentModel.szDesc, NULL, sizeof(m_CurrentModel.szDesc));
		}

		//경로명
		if (!GetRegString(path, _T("PATH"), (LPBYTE)m_CurrentModel.szPath))
		{
			memset(m_CurrentModel.szPath, NULL, sizeof(m_CurrentModel.szPath));
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}

	//현재생산데이타(null, 리스트에 존재하는지)
	str0.Empty();
	str0 = m_CurrentModel.szName;
	if (!str0.IsEmpty())
	{
		list_cnt = 0;
		list_cnt = m_listctrl_model.GetItemCount();
		for (i = 0; i < list_cnt; i++)
		{
			str1.Empty();
			str1 = m_listctrl_model.GetItemText(i, 0);
			if (0 == str0.Compare(str1))
			{
				str0.Empty();
				str0 = m_listctrl_model.GetItemText(i, 1);
				//sprintf_s(m_CurrentModel.szDesc, sizeof(m_CurrentModel.szDesc), ("%S"), (LPCTSTR)str0);
				sprintf(m_CurrentModel.szDesc, ("%S"), (LPCTSTR)str0);

				str0.Empty();
				str0 = m_listctrl_model.GetItemText(i, 2);

				//sprintf_s(m_CurrentModel.szPath, sizeof(m_CurrentModel.szPath), ("%S"), (LPCTSTR)str0);
				sprintf(m_CurrentModel.szPath, ("%S"), (LPCTSTR)str0);
				SelectList(i, TRUE);
				break;
			}
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}

	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogModel::OnPaint()
{
	// device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	CPaintDC dc(this);

	if (IsIconic())
	{

	}
	else
	{
		CRect rect;
		CPen pen, *oldPen;
		CBrush brush, *oldBrush;

		GetClientRect(&rect);

		//빈공간 채우기
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		oldPen = dc.SelectObject(&pen);
		brush.CreateSolidBrush(RGB(0, 0, 0));
		oldBrush = dc.SelectObject(&brush);

		dc.Rectangle(&rect);

		dc.SelectObject(oldPen);
		pen.DeleteObject();
		dc.SelectObject(oldBrush);
		brush.DeleteObject();

		CDialog::OnPaint();
	}
}

BEGIN_EVENTSINK_MAP(CDialogModel, CDialogEx)
	ON_EVENT(CDialogModel, IDC_BTNENHCTRL_MODEL_NEW, DISPID_CLICK, CDialogModel::ClickBtnenhctrlModelNew, VTS_NONE)
	ON_EVENT(CDialogModel, IDC_BTNENHCTRL_MODEL_SELECT, DISPID_CLICK, CDialogModel::ClickBtnenhctrlModelSelect, VTS_NONE)
	ON_EVENT(CDialogModel, IDC_BTNENHCTRL_MODEL_DELETE, DISPID_CLICK, CDialogModel::ClickBtnenhctrlModelDelete, VTS_NONE)
	ON_EVENT(CDialogModel, IDC_BTNENHCTRL_MODEL_OK, DISPID_CLICK, CDialogModel::ClickBtnenhctrlModelOk, VTS_NONE)
END_EVENTSINK_MAP()

void CDialogModel::ClickBtnenhctrlModelNew()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (IDYES == AfxMessageBox(_T("create a new model?"), MB_YESNO))
	{
		UpdateData(TRUE);

		BOOL name_not_null = TRUE;
		BOOL name_not_dupl = TRUE;

		typeModel mod;
		memset(&mod, NULL, sizeof(mod));

		CString str0, str1;
		int list_cnt, dupl_check;
		int i;

		WideCharToMultiByte(CP_ACP, 0, m_edit_strName.GetBuffer(), -1, mod.szName, 100, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, m_edit_strDescription.GetBuffer(), -1, mod.szDesc, 100, NULL, NULL);

		sprintf(mod.szPath, "%s\\%s", m_szMainPath, mod.szName);

		str0 = mod.szName;
		if (str0.IsEmpty())
		{
			name_not_null = FALSE;
		}

		//리스트에 같은 이름이 존재하는지 확인
		dupl_check = 0;
		list_cnt = 0;
		list_cnt = m_listctrl_model.GetItemCount();
		for (i = 0; i < list_cnt; i++)
		{
			str1.Empty();
			str1 = m_listctrl_model.GetItemText(i, 0);
			dupl_check = str0.Compare(str1);
			if (0 == dupl_check)
			{
				name_not_dupl = FALSE;
				break;
			}
		}

		if (name_not_null && name_not_dupl)
		{
			RemoveFileSRecurse((LPCTSTR)mod.szPath, -1);
			RemoveDirectory((LPCTSTR)mod.szPath);
			AddList(&mod);//1. Null이면 에러, 2. 같은 이름 있는지 확인 후 있으면 등록 불허
		}

		if (!name_not_null)
		{
			MessageBox(_T("Error : 제품명이 입력되지 않았습니다!"), _T("경고"), MB_OK | MB_ICONEXCLAMATION);
		}

		if (!name_not_dupl)
		{
			MessageBox(_T("Error : 신규등록 제품명이 이미 존재 합니다!"), _T("경고"), MB_OK | MB_ICONEXCLAMATION);
		}

		m_edit_strName.Empty();
		m_edit_strDescription.Empty();

		UpdateData(FALSE);
	}
}


void CDialogModel::ClickBtnenhctrlModelSelect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (IDYES == AfxMessageBox(_T("Change to the selected model?"), MB_YESNO))
	{
		CPMInspectDlg * pdlg = (CPMInspectDlg *)AfxGetApp()->m_pMainWnd;

		UpdateData(TRUE);

		CString str0, str1;
		int i, list_cnt, dupl_check;

		char szName[MAX_PATH] = { 0, };
		char szDesc[MAX_PATH] = { 0, };
		char szPath[MAX_PATH] = { 0, };

		typeModel mod;
		memset(&mod, NULL, sizeof(mod));

		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));

		WideCharToMultiByte(CP_ACP, 0, m_edit_strName.GetBuffer(), -1, mod.szName, 100, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, m_edit_strDescription.GetBuffer(), -1, mod.szDesc, 100, NULL, NULL);

		//sprintf_s(mod.szPath, sizeof(mod.szPath), "%s\\%s", m_szMainPath, mod.szName);
		sprintf(mod.szPath, "%s\\%s", m_szMainPath, mod.szName);

		//이름이 Null인지 확인 
		str0 = mod.szName;
		if (str0.IsEmpty())
		{
			MessageBox(_T("Error : 제품명이 입력되지 않았습니다!"), _T("경고"), MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		//리스트에 같은 이름 찾기
		BOOL find_check = FALSE;
		dupl_check = 0;
		list_cnt = 0;
		list_cnt = m_listctrl_model.GetItemCount();
		for (i = 0; i < list_cnt; i++)
		{
			str1.Empty();
			str1 = m_listctrl_model.GetItemText(i, 0);
			dupl_check = str0.Compare(str1);
			if ((0 == dupl_check) && !find_check)
			{
				//제품명
				WideCharToMultiByte(CP_ACP, 0, m_listctrl_model.GetItemText(i, 0).GetBuffer(), -1, m_CurrentModel.szName, 100, NULL, NULL);

				//설명
				WideCharToMultiByte(CP_ACP, 0, m_listctrl_model.GetItemText(i, 1).GetBuffer(), -1, m_CurrentModel.szDesc, 100, NULL, NULL);

				//경로
				WideCharToMultiByte(CP_ACP, 0, m_listctrl_model.GetItemText(i, 2).GetBuffer(), -1, m_CurrentModel.szPath, 100, NULL, NULL);

				SelectList(i, TRUE);

				find_check = TRUE;

				pdlg->SetChangeModel();	// 메인화면에서 변경된 모델명으로 변경한다.
			}
			else
			{
				SelectList(i, FALSE);
			}
		}

		if (!find_check)
			MessageBox(_T("Error : 변경할 제품명이 존재하지 않습니다!"), _T("경고"), MB_OK | MB_ICONEXCLAMATION);
	}
}


void CDialogModel::ClickBtnenhctrlModelDelete()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (IDYES == AfxMessageBox(_T("Delete to the selected model?"), MB_YESNO))
	{
		if (0 > m_iModelIdx)
			MessageBox(_T("Error : 제품이 선택되지 않았습니다!"), _T("경고"), MB_OK | MB_ICONEXCLAMATION);
		else
			m_listctrl_model.DeleteItem(m_iModelIdx);
	}
}


void CDialogModel::ClickBtnenhctrlModelOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CString path, str0, str1;
	int list_cnt, dupl_check;
	int i, idx;

	RegDeleteSubKey(HKEY_LOCAL_MACHINE, (LPCTSTR)m_szMainRegi);

	//20130201
	str1 = m_szMainPath;
	path.Empty();
	path.Format(_T("%S\\FULLPATH"), m_szMainRegi);
	str0 = _T("0");
	SetRegString(path, str0, str1);


	//레지스트리 적용하기
	//리스트 데이타
	dupl_check = 0;
	list_cnt = 0;
	list_cnt = m_listctrl_model.GetItemCount();

	idx = list_cnt;
	for (i = 0; i < list_cnt; i++)
	{
		idx--;
		str0.Empty();
		str0.Format(_T("%d"), i);

		//제품명
		path.Empty();
		path.Format(_T("%S\\NAME"), m_szMainRegi);
		str1.Empty();
		str1 = m_listctrl_model.GetItemText(idx, 0);
		if (!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}

		//설명
		path.Empty();
		path.Format(_T("%S\\DESCRIPTION"), m_szMainRegi);
		str1.Empty();
		str1 = m_listctrl_model.GetItemText(idx, 1);
		SetRegString(path, str0, str1);

		//경로명
		path.Empty();
		path.Format(_T("%S\\PATH"), m_szMainRegi);
		str1.Empty();
		str1 = m_listctrl_model.GetItemText(idx, 2);
		if (!str1.IsEmpty())
		{
			SetRegString(path, str0, str1);
		}
	}

	//20130201
	//리스트에 존재 하지 않는 모델 폴더 삭제
	typeModel mod;
	memset(&mod, NULL, sizeof(mod));
	BOOL bsame = FALSE;

	CFileFind finder;

	CString mainpath;
	mainpath.Format(_T("%S"), m_szMainPath);
	CString nextpath, temppath;

	mainpath += _T("\\*.*");

	BOOL bWorking = finder.FindFile(mainpath);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!bWorking)
			break;

		if (finder.IsDots())
			continue;


		if (!finder.IsDirectory())
			continue;

		nextpath = finder.GetFilePath();

		for (i = 0; i < list_cnt; i++)
		{
			str0.Empty();
			str0.Format(_T("%d"), i);

			//제품명
			path.Empty();
			path.Format(_T("%S\\PATH"), m_szMainRegi);
			if (!GetRegString(path, str0, (LPBYTE)mod.szPath))
			{
				break;
			}

			temppath = mod.szPath;

			if (nextpath.Compare(temppath) == 0)
			{
				bsame = TRUE;
			}

		}

		if (!bsame)
		{
			DeleteDirectory(nextpath);
		}

		bsame = FALSE;
	}

	//현재생산데이타(null, 리스트에 존재하는지)
	str0.Empty();
	str0 = m_CurrentModel.szName;
	BOOL chk = FALSE;
	if (!str0.IsEmpty())
	{
		list_cnt = 0;
		list_cnt = m_listctrl_model.GetItemCount();
		for (i = 0; i < list_cnt; i++)
		{
			str1.Empty();
			str1 = m_listctrl_model.GetItemText(i, 0);
			if (0 == str0.Compare(str1))
			{
				str0.Empty();
				str0 = m_listctrl_model.GetItemText(i, 1);
				WideCharToMultiByte(CP_ACP, 0, str0.GetBuffer(), -1, m_CurrentModel.szDesc, 100, NULL, NULL);

				str0.Empty();
				str0 = m_listctrl_model.GetItemText(i, 2);
				WideCharToMultiByte(CP_ACP, 0, str0.GetBuffer(), -1, m_CurrentModel.szPath, 100, NULL, NULL);
				SelectList(i, TRUE);
				chk = TRUE;
				break;
			}
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}
	if (!chk)
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}

	//현생산모델(인덱스 = -1)
	//제품명
	path.Empty();
	path.Format(_T("%S\\CURRENT"), m_szMainRegi);
	str1.Empty();
	str1 = m_CurrentModel.szName;
	SetRegString(path, _T("NAME"), str1);

	//설명
	str1.Empty();
	str1 = m_CurrentModel.szDesc;
	SetRegString(path, _T("DESCRIPTION"), str1);

	//경로명
	str1.Empty();
	str1 = m_CurrentModel.szPath;
	SetRegString(path, _T("PATH"), str1);
	/*
	switch (m_iDlgType)
	{
		//신규
	case 0:
		SetWindowText(_T("Model-New"));

		break;
		//삭제 
	case 1:
		SetWindowText(_T("Model-Delete"));

		break;
		//변경
	case 2:
		SetWindowText(_T("Model-Select"));
		m_edit_name.Format(_T("%S"), m_CurrentModel.szName);
		m_edit_desc.Format(_T("%S"), m_CurrentModel.szDesc);
		break;
		//Conversion
	case 3:
		SetWindowText(_T("Conversion Model-Select"));
		m_edit_name.Format(_T("%S"), m_CurrentModel.szName);
		m_edit_desc.Format(_T("%S"), m_CurrentModel.szDesc);
		break;
	default:
		AfxMessageBox(_T("Error : invalid index!"));
	}


	EndDialog(1);
	*/
}

void CDialogModel::SetModelPath(CString _path)
{
	CString path, str0;

	_path += MOD_MODEL_PATH;

	//sprintf_s(m_szMainPath, sizeof(m_szMainPath), ("%S"), _path);
	sprintf(m_szMainPath, ("%S"), _path);

	path.Empty();
	path.Format(_T("%S\\FULLPATH"), m_szMainRegi);

	str0 = _T("0");

	SetRegString(path, str0, _path);
}

void CDialogModel::Open()
{
	CString path;

	//현재생산데이타 불러오기
	memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));

	//제품명
	path.Empty();
	path.Format(_T("%S\\CURRENT"), m_szMainRegi);
	if (GetRegString(path, _T("NAME"), (LPBYTE)m_CurrentModel.szName))
	{
		Sleep(0);
		//설명
		if (!GetRegString(path, _T("DESCRIPTION"), (LPBYTE)m_CurrentModel.szDesc))
		{
			memset(m_CurrentModel.szDesc, NULL, sizeof(m_CurrentModel.szDesc));
		}
		Sleep(0);
		//경로명
		if (!GetRegString(path, _T("PATH"), (LPBYTE)m_CurrentModel.szPath))
		{
			memset(m_CurrentModel.szPath, NULL, sizeof(m_CurrentModel.szPath));
		}
	}
	else
	{
		memset(&m_CurrentModel, NULL, sizeof(m_CurrentModel));
	}
}

//20180727 데이터 파일명 관련 협의 필요. -by
BOOL CDialogModel::DeleteDirectory(CString lpDirPath)
{
	BOOL bRval = FALSE;
	int nRval = 0;
	CString szNextDirPath = _T("");
	CString szRoot = _T("");
	CString filename = _T("System.par");
	CString filename1 = _T("Inspection.par");
	CFileFind find;

	// 폴더가 존재 하는 지 확인 검사   
	bRval = find.FindFile(lpDirPath);

	if (bRval == FALSE)
		return bRval;

	while (bRval)
	{
		bRval = find.FindNextFile();
	
		// . or .. 인 경우 무시 한다.     
		if (find.IsDots() == TRUE)
		{
			continue;
		}
		// Directory 일 경우      
		if (find.IsDirectory())
		{
			szNextDirPath.Format(_T("%s\\*.*"), find.GetFilePath());

			// Recursion function 호 출          
			DeleteDirectory(szNextDirPath);
		}
		// file일 경우        
		else
		{
			// 파일 삭제  
			if (filename != find.GetFileName() || filename1 != find.GetFileName())
				::DeleteFile(find.GetFilePath());
		}
	}

	szRoot = find.GetRoot();
	find.Close();
	bRval = RemoveDirectory(szRoot);

	return bRval;
}

BOOL CDialogModel::AddList(typeModel* mod)
{
	LV_ITEM lvitem;
	BSTR bstr;
	CString temp;

	//	temp.Format(_T("%S"), mod->szName);
	temp = mod->szName;
	bstr = temp.AllocSysString();

	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = 0;
	lvitem.iSubItem = 0;
	lvitem.iImage = 1;
	lvitem.pszText = (LPWSTR)bstr;
	m_listctrl_model.InsertItem(&lvitem);
	SysFreeString(bstr);

	//	temp.Format(_T("%S"), mod->szDesc);
	temp.Empty();
	temp = mod->szDesc;
	bstr = temp.AllocSysString();

	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = 0;
	lvitem.iSubItem = 1;
	lvitem.pszText = (LPWSTR)bstr;//mod->szDesc;
	m_listctrl_model.SetItem(&lvitem);
	SysFreeString(bstr);

	//	temp.Format(_T("%S"), mod->szPath);
	temp.Empty();
	temp = mod->szPath;
	bstr = temp.AllocSysString();

	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = 0;
	lvitem.iSubItem = 2;
	lvitem.pszText = (LPWSTR)bstr;//mod->szPath;
	m_listctrl_model.SetItem(&lvitem);
	SysFreeString(bstr);

	return TRUE;
}

void CDialogModel::OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iModelIdx = 0;
	CString str;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_iModelIdx = pNMListView->iItem;

	str.Empty();

	//제품명
	str = m_listctrl_model.GetItemText(m_iModelIdx, 0);
	m_edit_strName = str;

	//설명
	str = m_listctrl_model.GetItemText(m_iModelIdx, 1);
	m_edit_strDescription = str;

	UpdateData(FALSE);

	*pResult = 0;
}

BOOL CDialogModel::SelectList(int idx, BOOL on_off)
{
	CString str;
	LV_ITEM lvitem;
	BSTR bstr;

	str.Empty();
	str = m_listctrl_model.GetItemText(idx, 0);
	bstr = str.AllocSysString();

	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = idx;
	lvitem.iSubItem = 0;
	lvitem.iImage = !(on_off);
	lvitem.pszText = (LPWSTR)bstr;
	m_listctrl_model.SetItem(&lvitem);

	SysFreeString(bstr);

	return TRUE;
}

BOOL CDialogModel::SetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, CString lpintValue)
{

	TCHAR ClassName[MAX_PATH] = { 0x00 }; // Buffer for class name.
	HKEY hkey;
	DWORD dwDesc = 0;
	DWORD temp;

	temp = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		lpstrKey,
		0,
		ClassName,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hkey,
		&dwDesc);

	// Set Registry Key & Value
	if (ERROR_SUCCESS == temp)
	{
		RegSetValueEx(hkey, lpstrEntry, 0, REG_SZ, (LPBYTE)lpintValue.GetBuffer(), lpintValue.GetLength() * sizeof(TCHAR));

		RegCloseKey(hkey);

		return TRUE;
	}

	return FALSE;
}

BOOL CDialogModel::GetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, BYTE *lpintValue)
{
	HKEY hkResult;
	DWORD dwLength = 512;// = DAT_BUF_LEN;
	DWORD dwType;// = 0;
	char strtemp[100];
	memset(strtemp, NULL, sizeof(100));

	TCHAR sval[100];
	memset(sval, NULL, sizeof(100));

	// Get Registry Key handle
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		lpstrKey,
		0,
		KEY_ALL_ACCESS,
		&hkResult)) {

		return FALSE;
	}
	else
	{

		// Get Entry Value
		if (ERROR_SUCCESS != RegQueryValueEx(hkResult,
			lpstrEntry,
			NULL,
			&dwType,
			(LPBYTE)sval,//lpintValue,
			&dwLength))
		{
			RegCloseKey(hkResult);
			return FALSE;
		}

		int nLengs = (int)(wcslen(sval) + 1) * 2;
		WideCharToMultiByte(CP_ACP, 0, sval, -1, (LPSTR)strtemp, nLengs, NULL, NULL);

		sprintf((char*)lpintValue, "%s", strtemp);


		RegCloseKey(hkResult);
	}
	return TRUE;
}

//레지스트리 전부삭제(하위 레지스트리까지 삭제)
int CDialogModel::RegDeleteSubKey(HKEY hKey, LPCTSTR lpSubKey)
{
	// RegDeleteKey() 함수는 NT이후 서브키가 있으면 삭제가 안된다.
	// 그래서 서브키까지 모두 삭제해주는 함수를 만들었다.

	HKEY newKey;
	char newSubKey[MAX_PATH];
	LONG Result;
	DWORD Size;
	FILETIME FileTime;

	RegOpenKeyEx(hKey, _T("SOFTWARE\\PMISYSTEM\\MODEL"), 0, KEY_ALL_ACCESS, &newKey);
	Result = ERROR_SUCCESS;

	while (TRUE)
	{
		Size = MAX_PATH;
		// 계속 키가 삭제 되므로 dwIndex는 항상 0을 넣어주어야 한다.
		// 만약 for문으로 i를 증가시며 사용하면 하나지우고 하나 뛰어넘어 반이 남는다.
		Result = RegEnumKeyEx(newKey, 0, (LPWSTR)newSubKey, &Size, NULL, NULL, NULL, &FileTime);
		if (Result != ERROR_SUCCESS) break;
		Result = RegDeleteSubKey(newKey, (LPCTSTR)newSubKey);
		if (Result != ERROR_SUCCESS) break;
	}

	RegCloseKey(newKey);

	return RegDeleteKey(hKey, lpSubKey);
}

void CDialogModel::RemoveFileSRecurse(LPCTSTR pstr, long period)
{
	CFileFind finder;
	//FILETIME prevT, file_creationtime;
	CTime current_time, file_creationtime;
	CTimeSpan cur_spantime, file_spantime;
	LONGLONG cur_days, file_creationdays, chk_file, max_file;
	max_file = LONG_MIN;

	// build a string with wildcards
	CString strWildcard(pstr), OldFName, path;
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it
		if (finder.IsDirectory() != 0)
		{
			current_time = CTime::GetCurrentTime();
			cur_spantime = current_time.GetTime();
			cur_days = cur_spantime.GetDays();

			//File creation days
			finder.GetCreationTime(file_creationtime);
			file_spantime = file_creationtime.GetTime();
			file_creationdays = file_spantime.GetDays();

			chk_file = cur_days - file_creationdays;

			CString str = finder.GetFilePath();
			//			AfxMessageBox(str);

			if (chk_file > ((LONG)period))
			{
				CString str = finder.GetFilePath();
				RemoveFileSRecurse(str, period);
				//				str += "(Directory 지우기 동작)";
				//				AfxMessageBox(str);
				RemoveDirectory(str);
			}
			continue;
		}
		else
		{
			//Current days
			current_time = CTime::GetCurrentTime();
			cur_spantime = current_time.GetTime();
			cur_days = cur_spantime.GetDays();

			//File creation days
			finder.GetCreationTime(file_creationtime);
			file_spantime = file_creationtime.GetTime();
			file_creationdays = file_spantime.GetDays();

			/////060209
			chk_file = cur_days - file_creationdays;
			if (chk_file > ((LONG)period))
			{
				path.Format(_T("%S"), finder.GetFilePath());
				if (path != """")
				{
					CFile::Remove((LPCTSTR)path);
				}
			}
		}
	}

}