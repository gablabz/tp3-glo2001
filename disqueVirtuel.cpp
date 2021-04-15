/**
 * \file disqueVirtuel.cpp
 * \brief Implémentation d'un disque virtuel.
 * \author ?
 * \version 0.1
 * \date  2021
 *
 *  Travail pratique numéro 3
 *
 */

#include "disqueVirtuel.h"
#include <iostream>
#include <string>
//vous pouvez inclure d'autres librairies si c'est nécessaire

namespace TP3
{

	int DisqueVirtuel::bd_FormatDisk(){
		m_blockDisque.clear();
		initBitmapEmptyBlocks();
		initBitmapEmptyINodes();
		initINodes();

	}


	void DisqueVirtuel::initBitmapEmptyBlocks(){

		Block bitmapBlock = Block(S_IFBL);

		//Marquer les blocs 0 a 23 comme occupés, le reste comme libre.
		for (int i=0; i<(BASE_BLOCK_INODE + N_INODE_ON_DISK); i++){
			bitmapBlock.m_bitmap[i] = false;
		}
		for (int j=BASE_BLOCK_INODE + N_INODE_ON_DISK; j<N_BLOCK_ON_DISK; j++){
			bitmapBlock.m_bitmap[j] = true;
		}

		//Ajouter le bitmap des blocs libres sur le disque.
		m_blockDisque[FREE_BLOCK_BITMAP] = bitmapBlock;
	}


	void DisqueVirtuel::initBitmapEmptyINodes(){
		Block bitmapINodes = Block(S_IFIL);

		//Marquer l'i-node 1 comme occupé, le reste comme libre.
		bitmapINodes.m_bitmap[0] = false;
		for (int i=1; i<N_INODE_ON_DISK; i++){
			bitmapINodes.m_bitmap[i] = true;
		}
		//Ajouter le bitmap des i-nodes libres sur le disque.
		m_blockDisque[FREE_INODE_BITMAP] = bitmapINodes;
	}

	void DisqueVirtuel::initINodes(){
		for (int i=BASE_BLOCK_INODE; i<(BASE_BLOCK_INODE + N_INODE_ON_DISK); i++){
			m_blockDisque[i] = Block(S_IFIN);
		}
	}




}//Fin du namespace
