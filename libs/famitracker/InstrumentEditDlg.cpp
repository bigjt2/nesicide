/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2012  Jonathan Liss
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

#include "stdafx.h"
#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "FamiTrackerView.h"
#include "InstrumentEditDlg.h"
#include "InstrumentEditor2A03.h"
#include "InstrumentEditorDPCM.h"
#include "InstrumentEditorVRC6.h"
#include "InstrumentEditorVRC7.h"
#include "InstrumentEditorFDS.h"
#include "InstrumentEditorFDSEnvelope.h"
#include "InstrumentEditorN163.h"
#include "InstrumentEditorN163Wave.h"
#include "InstrumentEditorS5B.h"
#include "MainFrm.h"

// Constants
const int CInstrumentEditDlg::KEYBOARD_TOP	  = 323;
const int CInstrumentEditDlg::KEYBOARD_LEFT	  = 12;
const int CInstrumentEditDlg::KEYBOARD_WIDTH  = 561;
const int CInstrumentEditDlg::KEYBOARD_HEIGHT = 58;

const TCHAR *CInstrumentEditDlg::CHIP_NAMES[] = {_T(""), _T("2A03"), _T("VRC6"), _T("VRC7"), _T("FDS"), _T("Namco"), _T("Sunsoft")};

// CInstrumentEditDlg dialog

IMPLEMENT_DYNAMIC(CInstrumentEditDlg, CDialog)

CInstrumentEditDlg::CInstrumentEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstrumentEditDlg::IDD, pParent),
	m_bOpened(false),
	m_iInstrument(-1)
{
}

void CInstrumentEditDlg::instTab_currentChanged(int arg1)
{
   NMHDR nmhdr;
   LRESULT result;
   OnTcnSelchangeInstTab(&nmhdr,&result);
}

CInstrumentEditDlg::~CInstrumentEditDlg()
{
}

void CInstrumentEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//BEGIN_MESSAGE_MAP(CInstrumentEditDlg, CDialog)
//	ON_NOTIFY(TCN_SELCHANGE, IDC_INST_TAB, OnTcnSelchangeInstTab)
//	ON_WM_PAINT()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONDBLCLK()
//	ON_WM_NCLBUTTONUP()
//END_MESSAGE_MAP()


// CInstrumentEditDlg message handlers

BOOL CInstrumentEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iSelectedInstType = -1;
	m_iLastKey = -1;
	m_bOpened = true;
	m_iPanels = 0;

	for (int i = 0; i < PANEL_COUNT; ++i)
		m_pPanels[i] = NULL;

	// Get active document
	CFrameWnd *pFrameWnd = static_cast<CFrameWnd*>(GetParent());
	m_pDocument = static_cast<CFamiTrackerDoc*>(pFrameWnd->GetActiveDocument());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentEditDlg::InsertPane(CInstrumentEditPanel *pPanel, bool Show)
{
	CRect Rect, ParentRect;
	CTabCtrl *pTabControl = static_cast<CTabCtrl*>(GetDlgItem(IDC_INST_TAB));

	pTabControl->GetWindowRect(&ParentRect);
	pTabControl->InsertItem(m_iPanels, pPanel->GetTitle());

	pPanel->Create(pPanel->GetIDD(), this);
	pPanel->GetWindowRect(&Rect);
	Rect.MoveToXY(ParentRect.left - Rect.left + SX(1), ParentRect.top - Rect.top + SY(21));
	Rect.bottom -= SY(2);
	Rect.right += SX(1);
	pPanel->MoveWindow(Rect);
	pPanel->ShowWindow(Show ? SW_SHOW : SW_HIDE);

	if (Show) {
		pTabControl->SetCurSel(m_iPanels);
		pPanel->SetFocus();
		m_pFocusPanel = pPanel;
	}

	m_pPanels[m_iPanels++] = pPanel;
}

void CInstrumentEditDlg::ClearPanels()
{
	static_cast<CTabCtrl*>(GetDlgItem(IDC_INST_TAB))->DeleteAllItems();

	for (int i = 0; i < PANEL_COUNT; i++) {
		if (m_pPanels[i] != NULL) {
			m_pPanels[i]->DestroyWindow();
			SAFE_RELEASE(m_pPanels[i]);
		}
	}

	m_iPanels = 0;
	m_iInstrument = -1;
}

void CInstrumentEditDlg::SetCurrentInstrument(int Index)
{
	CString Title;
	char Name[256];
	CInstrument *pInst = m_pDocument->GetInstrument(Index);
	int InstType = pInst->GetType();

	// Dialog title
	m_pDocument->GetInstrumentName(Index, Name);	
   Title.Format(IDS_INST_EDITOR_TITLE, Index, Name, CHIP_NAMES[InstType]);
	SetWindowText(Title);

	if (InstType != m_iSelectedInstType) {

		ShowWindow(SW_HIDE);

		ClearPanels();
      
		switch (InstType) {
			case INST_2A03: {
					int Channel = CFamiTrackerView::GetView()->GetSelectedChannel();
					int Type = CFamiTrackerDoc::GetDoc()->GetChannelType(Channel);
					bool bShowDPCM = (Type == CHANID_DPCM) || (((CInstrument2A03*)pInst)->AssignedSamples());
               InsertPane(new CInstrumentEditor2A03(), !bShowDPCM);
					InsertPane(new CInstrumentEditorDPCM(), bShowDPCM);
				}
				break;
			case INST_VRC6:
				InsertPane(new CInstrumentEditorVRC6(), true);
				break;
			case INST_VRC7:
				InsertPane(new CInstrumentEditorVRC7(), true);
				break;
			case INST_FDS:
				InsertPane(new CInstrumentEditorFDS(), true);
				InsertPane(new CInstrumentEditorFDSEnvelope(), false);
				break;
			case INST_N163:
				InsertPane(new CInstrumentEditorN163(), true);
				InsertPane(new CInstrumentEditorN163Wave(), false);
				break;
			case INST_S5B:
				InsertPane(new CInstrumentEditorS5B(), true);
				break;
		}

		m_iSelectedInstType = InstType;
	}

	for (int i = 0; i < PANEL_COUNT; ++i) {
		if (m_pPanels[i] != NULL) {
			m_pPanels[i]->SelectInstrument(Index);
		}
	}

	ShowWindow(SW_SHOW);
	UpdateWindow();

	m_iSelectedInstType = InstType;
}

void CInstrumentEditDlg::OnTcnSelchangeInstTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTabCtrl *pTabControl = (CTabCtrl*)GetDlgItem(IDC_INST_TAB);
	int Selection = pTabControl->GetCurSel();
   
	for (int i = 0; i < PANEL_COUNT; i++) {
		if (m_pPanels[i] != NULL) {
			m_pPanels[i]->ShowWindow(SW_HIDE);
		}
	}

	m_pPanels[Selection]->ShowWindow(SW_SHOW);
	m_pPanels[Selection]->SetFocus();

	m_pFocusPanel = m_pPanels[Selection];

	*pResult = 0;
}

void CInstrumentEditDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Do not call CDialog::OnPaint() for painting messages

	const int WHITE_KEY_W	= 10;
	const int BLACK_KEY_W	= 8;

	CBitmap WhiteKeyBmp, BlackKeyBmp, *OldWhite;
	CBitmap WhiteKeyMarkBmp, BlackKeyMarkBmp, *OldBlack;

	CDC WhiteKey, BlackKey;
	CDC BackDC;

	CBitmap Bmp, *OldBmp;

	Bmp.CreateCompatibleBitmap(&dc, 800, 800);
	BackDC.CreateCompatibleDC(&dc);
	OldBmp = BackDC.SelectObject(&Bmp);

	WhiteKeyBmp.LoadBitmap(IDB_KEY_WHITE);
	BlackKeyBmp.LoadBitmap(IDB_KEY_BLACK);
	WhiteKeyMarkBmp.LoadBitmap(IDB_KEY_WHITE_MARK);
	BlackKeyMarkBmp.LoadBitmap(IDB_KEY_BLACK_MARK);

	WhiteKey.CreateCompatibleDC(&dc);
	BlackKey.CreateCompatibleDC(&dc);

	OldWhite = WhiteKey.SelectObject(&WhiteKeyBmp);
	OldBlack = BlackKey.SelectObject(&BlackKeyBmp);

	int Pos;

	const int WHITE[]	= {0, 2, 4, 5, 7, 9, 11};
	const int BLACK_1[] = {1, 3};
	const int BLACK_2[] = {6, 8, 10};

	int Note	= m_iActiveKey % 12;
	int Octave	= m_iActiveKey / 12;

	for (int j = 0; j < 8; j++) {
		Pos = /*KEYBOARD_LEFT +*/ ((WHITE_KEY_W * 7) * j);

		for (int i = 0; i < 7; i++) {
			if ((Note == WHITE[i]) && (Octave == j) && m_iActiveKey != -1)
				WhiteKey.SelectObject(WhiteKeyMarkBmp);
			else
				WhiteKey.SelectObject(WhiteKeyBmp);

			BackDC.BitBlt(i * WHITE_KEY_W + Pos, 0, 100, 100, &WhiteKey, 0, 0, SRCCOPY);
		}

      for (int i = 0; i < 2; i++) {
			if ((Note == BLACK_1[i]) && (Octave == j) && m_iActiveKey != -1)
				BlackKey.SelectObject(BlackKeyMarkBmp);
			else
				BlackKey.SelectObject(BlackKeyBmp);

			BackDC.BitBlt(i * WHITE_KEY_W + WHITE_KEY_W / 2 + 1 + Pos, 0, 100, 100, &BlackKey, 0, 0, SRCCOPY);
		}

		for (int i = 0; i < 3; i++) {
			if ((Note == BLACK_2[i]) && (Octave == j) && m_iActiveKey != -1)
				BlackKey.SelectObject(BlackKeyMarkBmp);
			else
				BlackKey.SelectObject(BlackKeyBmp);

			BackDC.BitBlt((i + 3) * WHITE_KEY_W + WHITE_KEY_W / 2 + 1 + Pos, 0, 100, 100, &BlackKey, 0, 0, SRCCOPY);
		}
   }

	WhiteKey.SelectObject(OldWhite);
	BlackKey.SelectObject(OldBlack);

	dc.BitBlt(SX(KEYBOARD_LEFT - 6) + 6, SY(KEYBOARD_TOP - 12) + 12, KEYBOARD_WIDTH, KEYBOARD_HEIGHT, &BackDC, 0, 0, SRCCOPY);

	BackDC.SelectObject(OldBmp);
}

void CInstrumentEditDlg::ChangeNoteState(int Note)
{
	// A MIDI key number or -1 to disable

	m_iActiveKey = Note;

//	if (m_hWnd)
		RedrawWindow(CRect(KEYBOARD_LEFT, KEYBOARD_TOP, 580, KEYBOARD_TOP + 100), 0, RDW_INVALIDATE);
}

void CInstrumentEditDlg::SwitchOnNote(int x, int y)
{
	CFamiTrackerView *pView = CFamiTrackerView::GetView();
	stChanNote NoteData;
	int Octave;
	int Note;
	int KeyPos;

	// TODO: remove hardcoded numbers

	// Send to DPCM channel if DPCM view is activated
	if (m_iSelectedInstType == INST_2A03 && m_pPanels[1]->IsWindowVisible())
		pView->SelectChannel(4);

	// Select FDS channel
	if (m_iSelectedInstType == INST_FDS)
		pView->SelectChannel(5);

	int Channel = pView->GetSelectedChannel();

	Octave = (x - KEYBOARD_LEFT) / 70;

	if (y > KEYBOARD_TOP && y < (KEYBOARD_TOP + 58) && x > KEYBOARD_LEFT && x < (KEYBOARD_LEFT + 560)) {
		if (y > KEYBOARD_TOP + 38) {
			
			// Only white keys
			KeyPos = (x - KEYBOARD_LEFT) % 70;

			if (KeyPos >= 0 && KeyPos < 10)				// C
				Note = 0;
			else if (KeyPos >= 10 && KeyPos < 20)		// D
				Note = 2;
			else if (KeyPos >= 20 && KeyPos < 30)		// E
				Note = 4;
			else if (KeyPos >= 30 && KeyPos < 40)		// F
				Note = 5;
			else if (KeyPos >= 40 && KeyPos < 50)		// G
				Note = 7;
			else if (KeyPos >= 50 && KeyPos < 60)		// A
				Note = 9;
			else if (KeyPos >= 60 && KeyPos < 70)		// B
				Note = 11;
		}
		else {
			// Black and white keys
			KeyPos = (x - KEYBOARD_LEFT) % 70;

			if (KeyPos >= 0 && KeyPos < 7)			// C
				Note = 0;
			else if (KeyPos >= 7 && KeyPos < 13) 	// C#
				Note = 1;
			else if (KeyPos >= 13 && KeyPos < 16) 	// D
				Note = 2;
			else if (KeyPos >= 16 && KeyPos < 23) 	// D#
				Note = 3;
			else if (KeyPos >= 23 && KeyPos < 30) 	// E
				Note = 4;
			else if (KeyPos >= 30 && KeyPos < 37) 	// F
				Note = 5;
			else if (KeyPos >= 37 && KeyPos < 43) 	// F#
				Note = 6;
			else if (KeyPos >= 43 && KeyPos < 46) 	// G
				Note = 7;
			else if (KeyPos >= 46 && KeyPos < 53) 	// G#
				Note = 8;
			else if (KeyPos >= 53 && KeyPos < 56) 	// A
				Note = 9;
			else if (KeyPos >= 56 && KeyPos < 62) 	// A#
				Note = 10;
			else if (KeyPos >= 62 && KeyPos < 70) 	// B
				Note = 11;
		}

		if (Note + (Octave * 12) != m_iLastKey) {
			NoteData.Note			= Note + 1;
			NoteData.Octave			= Octave;
			NoteData.Vol			= 0x0F;
			NoteData.Instrument		= pView->GetInstrument();
			memset(NoteData.EffNumber, 0, 4);
			memset(NoteData.EffParam, 0, 4);

			pView->FeedNote(Channel, &NoteData);
		}

		m_iLastKey = Note + (Octave * 12);
	}
	else {
		NoteData.Note			= pView->DoRelease() ? RELEASE : HALT;//HALT;
		NoteData.Octave			= 0;
		NoteData.Vol			= 0;
		NoteData.Instrument		= 0;
		memset(NoteData.EffNumber, 0, 4);
		memset(NoteData.EffParam, 0, 4);

		pView->FeedNote(Channel, &NoteData);

		m_iLastKey = -1;
	}
}

void CInstrumentEditDlg::SwitchOffNote(bool ForceHalt)
{
	stChanNote NoteData;

	CFamiTrackerView *pView = CFamiTrackerView::GetView();

	int Channel = pView->GetSelectedChannel();

	NoteData.Note			= (pView->DoRelease() && !ForceHalt) ? RELEASE : HALT;
	NoteData.Octave			= 0;
	NoteData.Vol			= 0x10;
	NoteData.Instrument		= pView->GetInstrument();// 0;
	memset(NoteData.EffNumber, 0, 4);
	memset(NoteData.EffParam, 0, 4);

	pView->FeedNote(Channel, &NoteData);

	m_iLastKey = -1;
}

void CInstrumentEditDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	ScaleMouse(point);
	SwitchOnNote(point.x, point.y);
	CDialog::OnLButtonDown(nFlags, point);
}

void CInstrumentEditDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	SwitchOffNote(false);
	CDialog::OnLButtonUp(nFlags, point);
}

void CInstrumentEditDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	ScaleMouse(point);

	if (nFlags & MK_LBUTTON)
		SwitchOnNote(point.x, point.y);

	CDialog::OnMouseMove(nFlags, point);
}

void CInstrumentEditDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SwitchOnNote(point.x, point.y);
	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CInstrumentEditDlg::DestroyWindow()
{	
	ClearPanels();

	m_iSelectedInstType = -1;
	m_iInstrument = -1;
	m_bOpened = false;
	
	return CDialog::DestroyWindow();
}

void CInstrumentEditDlg::OnOK()
{
//	DestroyWindow();
//	CDialog::OnOK();
}

void CInstrumentEditDlg::OnCancel()
{
	DestroyWindow();
}

void CInstrumentEditDlg::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	// Mouse was released outside the client area
	SwitchOffNote(true);
	CDialog::OnNcLButtonUp(nHitTest, point);
}

bool CInstrumentEditDlg::IsOpened() const
{
	return m_bOpened;
}

void CInstrumentEditDlg::PostNcDestroy()
{
	// TODO Use this function to destroy the panels so it won't be visible when closing the editor
	CDialog::PostNcDestroy();
}

void CInstrumentEditDlg::mouseMoveEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   OnMouseMove(flags,point);
}

void CInstrumentEditDlg::mousePressEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      OnLButtonDown(flags,point);
   }
}

void CInstrumentEditDlg::mouseReleaseEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      OnLButtonUp(flags,point);
   }
}

void CInstrumentEditDlg::mouseDoubleClickEvent(QMouseEvent *event)
{
   CPoint point(event->pos());
   unsigned int flags = 0;
   if ( event->modifiers()&Qt::ControlModifier )
   {
      flags |= MK_CONTROL;
   }
   if ( event->modifiers()&Qt::ShiftModifier )
   {
      flags |= MK_SHIFT;
   }
   if ( event->buttons()&Qt::LeftButton )
   {
      flags |= MK_LBUTTON;
   }
   if ( event->buttons()&Qt::MiddleButton )
   {
      flags |= MK_MBUTTON;
   }
   if ( event->buttons()&Qt::RightButton )
   {
      flags |= MK_RBUTTON;            
   }
   if ( event->button() == Qt::LeftButton )
   {
      OnLButtonDblClk(flags,point);
   }
}
