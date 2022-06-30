/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
This file is part of Quake III Arena source code.
Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.
Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "STDInclude.hpp"
#include "Extern/Huffman.hpp"

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
* Compression book.  The ranks are not actually stored, but implicitly defined
* by the location of a node within a doubly-linked list */

static int bloc = 0;

/* Add a bit to the output file (buffered) */
static void add_bit(char bit, byte *fout) {
	if ((bloc & 7) == 0) {
		fout[(bloc >> 3)] = 0;
	}
	fout[(bloc >> 3)] |= bit << (bloc & 7);
	bloc++;
}

/* Receive one bit from the input file (buffered) */
static int get_bit(byte *fin) {
	int t;
	t = (fin[(bloc >> 3)] >> (bloc & 7)) & 0x1;
	bloc++;
	return t;
}

/* Get a symbol */

static void Huff_offsetReceive(node_t *node, int *ch, byte *fin, int *offset) {
	bloc = *offset;
	while (node && node->symbol == INTERNAL_NODE) {

		if (get_bit(fin)) {
			node = node->right;

		}
		else {
			node = node->left;

		}
	}
	if (!node) {
		*ch = 0;
		//		Com_PrintError("Illegal tree!\n");
		return;

	}
	*ch = node->symbol;
	*offset = bloc;
}



/* Send the prefix code for this node */
static void Huff_send(node_t *node, node_t *child, byte *fout) {
	if (node->parent) {
		Huff_send(node->parent, node, fout);
	}
	if (child) {
		if (node->right == child) {
			add_bit(1, fout);
		}
		else {
			add_bit(0, fout);
		}
	}
}

static void Huff_offsetTransmit(huff_t *huff, int ch, byte *fout, int *offset) {
	bloc = *offset;
	Huff_send(huff->loc[ch], NULL, fout);
	*offset = bloc;
}


static void Huff_Init(huff_t *huff) {

	Com_Memset(huff, 0, sizeof(huff_t));

	// Initialize the tree & list with the NYT node
	huff->tree = &(huff->nodeList[huff->blocNode++]);
	huff->loc[NYT] = huff->tree;

	huff->tree->symbol = NYT;

	huff->tree->weight = 0;
	huff->tree->parent = NULL;
	huff->tree->left = NULL;
	huff->tree->right = NULL;
}



int _msg_hData[256] = {
	0x384E8,
	0x7A5A,
	0x5068,
	0x60CA,
	0x4F54,
	0x4203,
	0x49ED,
	0x3191,
	0x2ECD,
	0x1E95,
	0x1D04,
	0x1062,
	0x1CAE,
	0x17B1,
	0x16D3,
	0x1699,
	0x2471,
	0x161A,
	0x15C3,
	0x1058,
	0x18E3,
	0x1668,
	0x0F7B,
	0x0E24,
	0x15DC,
	0x10C5,
	0x0EEE,
	0x0D2D,
	0x1163,
	0x18F3,
	0x0E82,
	0x11E1,
	0x647F,
	0x109F,
	0x1B2B,
	0x0A23,
	0x13D3,
	0x0E67,
	0x0DE5,
	0x0B20,
	0x17CA,
	0x0DE7,
	0x0DCE,
	0x0D52,
	0x0DF8,
	0x0CD5,
	0x0C24,
	0x0AFA,
	0x40D8,
	0x23BE,
	0x2330,
	0x1374,
	0x18D2,
	0x15E1,
	0x1576,
	0x0FF8,
	0x1474,
	0x117E,
	0x0C14,
	0x86C,
	0x0B6E,
	0x0AAD,
	0x0BC6,
	0x16E5,
	0x45FE,
	0x1331,
	0x1777,
	0x12A3,
	0x16E3,
	0x1AE6,
	0x0A6C,
	0x0C21,
	0x151A,
	0x0F03,
	0x0F02,
	0x0E8F,
	0x14C1,
	0x0B68,
	0x0D91,
	0x0CFF,
	0x1C6A,
	0x0B74,
	0x0ECC,
	0x0B5B,
	0x0DB4,
	0x0B95,
	0x0B70,
	0x0A69,
	0x110D,
	0x0AA9,
	0x1218,
	0x97B,
	0x0AAE,
	0x75E,
	0x6DF,
	0x1219,
	0x1A22,
	0x1D65,
	0x0FA1,
	0x0DE8,
	0x1A68,
	0x11D0,
	0x1026,
	0x0A15,
	0x0E62,
	0x0D82,
	0x0A2B,
	0x824,
	0x13B5,
	0x0B3C,
	0x0B2E,
	0x9C1,
	0x2AA7,
	0x22C6,
	0x1027,
	0x0C8A,
	0x1003,
	0x0B1B,
	0x0A52,
	0x8CE,
	0x0D11,
	0x9D1,
	0x0ADC,
	0x773,
	0x0C66,
	0x0A24,
	0x9E4,
	0x1686,
	0x5E95,
	0x1B67,
	0x138B,
	0x189E,
	0x0F58,
	0x0DFA,
	0x0D3F,
	0x1936,
	0x13D2,
	0x0BDB,
	0x0BF6,
	0x0A32,
	0x0C36,
	0x841,
	0x9E2,
	0x0B3D,
	0x1602,
	0x0C22,
	0x0D7F,
	0x0AF7,
	0x12C6,
	0x914,
	0x9CE,
	0x98C,
	0x0EA7,
	0x0C4E,
	0x848,
	0x77A,
	0x981,
	0x716,
	0x893,
	0x0D28,
	0x20AA,
	0x9A1,
	0x0A23,
	0x850,
	0x0CFF,
	0x0AA3,
	0x0A7D,
	0x976,
	0x0EBC,
	0x853,
	0x8ED,
	0x943,
	0x0D56,
	0x8D9,
	0x8EE,
	0x877,
	0x15F1,
	0x0A8C,
	0x0AEE,
	0x731,
	0x8CF,
	0x679,
	0x0AFC,
	0x52B,
	0x0ADB,
	0x6B7,
	0x784,
	0x62F,
	0x963,
	0x681,
	0x93B,
	0x0B9B,
	0x251F,
	0x1467,
	0x0E03,
	0x0EF2,
	0x1053,
	0x0D55,
	0x0D69,
	0x0BDE,
	0x1140,
	0x924,
	0x930,
	0x721,
	0x0A91,
	0x669,
	0x7B4,
	0x89F,
	0x0F91,
	0x7B9,
	0x0A2D,
	0x8E7,
	0x0C56,
	0x76E,
	0x81D,
	0x5E9,
	0x12A2,
	0x7BC,
	0x6F4,
	0x68C,
	0x77E,
	0x78F,
	0x900,
	0x8AB,
	0x1579,
	0x8AD,
	0x0A9B,
	0x8AA,
	0x0BA8,
	0x88B,
	0x7EF,
	0x9BB,
	0x10E9,
	0x0B61,
	0x0AB6,
	0x0A09,
	0x0C6B,
	0x81D,
	0x7DE,
	0x9A2,
	0x0F5D,
	0x9AC,
	0x941,
	0x7D4,
	0x0C80,
	0x999,
	0x0ADD,
	0x97F,
	0x13FC,
	0x97F,
	0x0CFC,
	0x0B30,
	0x11C2,
	0x0E4B,
	0x1592,
	0x753D
};

static huff_t		msgHuff;

int MSG_ReadBitsCompress(const byte* input, int readsize, byte* outputBuf, int outputBufSize) {

	readsize = readsize * 8;
	byte *outptr = outputBuf;

	int get;
	int offset;
	int i;

	if (readsize <= 0) {
		return 0;
	}

	for (offset = 0, i = 0; offset < readsize && i < outputBufSize; i++) {
		Huff_offsetReceive(msgHuff.tree, &get, (byte*)input, &offset);
		*outptr = (byte)get;
		outptr++;
	}
	return i;
}

int MSG_WriteBitsCompress(char /*dummy*/, const byte *datasrc, byte *buffdest, int bytecount) {

	int offset;
	int i;

	if (bytecount <= 0) {
		return 0;
	}

	for (offset = 0, i = 0; i < bytecount; i++) {
		Huff_offsetTransmit(&msgHuff, (int)datasrc[i], buffdest, &offset);
	}
	return (offset + 7) / 8;
}

int __cdecl nodeCmp(const void *left, const void *right)
{
	return reinterpret_cast<const nodetype*>(left)->left->weight - reinterpret_cast<const nodetype*>(right)->left->weight;
}

static void Huff_BuildFromData(huff_t* huff, const int* msg_hData)
{
	static_assert(sizeof(intptr_t) == 4, "64bit support not yet given!");

	huff_t *v2; // esi
	signed int v3; // edx
	const int *v4; // eax
	int v5; // ebx
	int v6; // ecx
	int v7; // ecx
	int v8; // ebx
	nodetype *v9; // ecx
	int v10; // ebx
	int v11; // ecx
	int v12; // ebx
	int v13; // ecx
	int v14; // ecx
	int v15; // ebx
	nodetype *v16; // ecx
	int v17; // ebx
	nodetype *v18; // eax
	nodetype *v19; // ecx
	nodetype **v20; // ebx
	nodetype *v21; // eax
	nodetype *v22; // ecx
	int v23; // ecx
	bool v24; // zf
	nodetype *heap[256]; // [esp+Ch] [ebp-420h]
	int v26; // [esp+40Ch] [ebp-20h]
	int v27; // [esp+410h] [ebp-1Ch]
	int v28; // [esp+414h] [ebp-18h]
	int v29; // [esp+418h] [ebp-14h]
	int v30; // [esp+41Ch] [ebp-10h]
	int v31; // [esp+420h] [ebp-Ch]
	int heapHead; // [esp+424h] [ebp-8h]
	nodetype **v33; // [esp+428h] [ebp-4h]
	int symbola; // [esp+434h] [ebp+8h]
	int symbolb; // [esp+434h] [ebp+8h]
	int symbolc; // [esp+434h] [ebp+8h]
	int symbold; // [esp+434h] [ebp+8h]
	int symbol; // [esp+434h] [ebp+8h]

	v26 = 8 - (_DWORD)msg_hData;
	v31 = 12 - (_DWORD)msg_hData;
	v30 = (char *)heap - (char *)msg_hData;
	v28 = 4 - (_DWORD)msg_hData;
	v29 = 16 - (_DWORD)msg_hData;
	v33 = &heap[1];
	v27 = (char *)&heap[1] - (char *)msg_hData;
	v2 = huff;
	heapHead = 0;
	v3 = 2;
	v4 = msg_hData + 2;
	do
	{
		v5 = v2->blocNode;
		symbola = *(v4 - 2);
		v6 = v2->blocNode++;
		v2->nodeList[v6].left = 0;
		v2->nodeList[v6].right = 0;
		v2->nodeList[v6].parent = 0;
		v7 = (int)&v2->nodeList[v5];
		*(_DWORD *)(v7 + 16) = v3 - 2;
		*(_DWORD *)(v7 + 12) = symbola;
		*(int *)((char *)&v2->blocNode + (_DWORD)v4 + v28) = v7;
		*(v33 - 1) = (nodetype *)v7;
		v8 = v2->blocNode;
		symbolb = *(v4 - 1);
		++v2->blocNode;
		v9 = &v2->nodeList[v8];
		v9->symbol = v3 - 1;
		v9->weight = symbolb;
		v10 = v26;
		v9->left = 0;
		v9->right = 0;
		v9->parent = 0;
		*(int *)((char *)&v2->blocNode + (_DWORD)v4 + v10) = (int)v9;
		*v33 = v9;
		symbolc = *v4;
		v11 = v2->blocNode++;
		v2->nodeList[v11].symbol = v3;
		v2->nodeList[v11].weight = symbolc;
		v12 = v31;
		v2->nodeList[v11].left = 0;
		v13 = (int)&v2->nodeList[v11];
		*(_DWORD *)(v13 + 4) = 0;
		*(_DWORD *)(v13 + 8) = 0;
		*(int *)((char *)&v2->blocNode + (_DWORD)v4 + v12) = v13;
		*(int *)((char *)v4 + v30) = v13;
		v14 = v4[1];
		v15 = v2->blocNode;
		v33 += 4;
		symbold = v14;
		v2->blocNode = v15 + 1;
		v16 = &v2->nodeList[v15];
		v16->symbol = v3 + 1;
		v16->weight = symbold;
		v17 = v29;
		v16->left = 0;
		v16->right = 0;
		v16->parent = 0;
		*(int *)((char *)&v2->blocNode + (_DWORD)v4 + v17) = (int)v16;
		v3 += 4;
		*(int *)((char *)v4 + v27) = (const int)v16;
		v4 += 4;
	} while (v3 - 2 < 256);
	qsort(heap, 0x100u, 4u, nodeCmp);
	v18 = &v2->nodeList[v2->blocNode++];
	v18->right = 0;
	v18->left = 0;
	v18->weight = 1;
	v18->symbol = 257;
	v18->parent = 0;
	v2->freelist = &v18->left;
	v19 = v2->tree;
	v18->left = v19;
	v18->right = heap[0];
	v19->parent = v18;
	v18->right->parent = v18;
	v20 = heap;
	v18->weight = v18->right->weight + v18->left->weight;
	heap[0] = v18;
	symbol = 0;
	v33 = (nodetype **)255;
	do
	{
		qsort(v20, 256 - heapHead, 4u, nodeCmp);
		v21 = &v2->nodeList[v2->blocNode++];
		v21->left = 0;
		v21->right = 0;
		v21->symbol = 257;
		v21->parent = 0;
		v21->weight = 1;
		v22 = *v20;
		v2->freelist = &v21->left;
		v21->left = v22;
		v21->right = *(nodetype **)((char *)&heap[1] + symbol);
		v21->left->parent = v21;
		v21->right->parent = v21;
		v21->weight = v21->right->weight + v21->left->weight;
		v23 = heapHead + 1;
		heapHead = v23;
		v23 *= 4;
		v24 = v33 == (nodetype **)1;
		v33 = (nodetype **)((char *)v33 - 1);
		v20 = (nodetype **)((char *)heap + v23);
		symbol = v23;
		*(nodetype **)((char *)heap + v23) = v21;
	} while (!v24);
	v2->tree = heap[heapHead];
}

void Huffman_InitMain() {

	static qboolean huffInit = false;

	if (huffInit)
		return;

	huffInit = true;
	Huff_Init(&msgHuff);
	Huff_BuildFromData(&msgHuff, _msg_hData);
}