/**
 * \file disqueVirtuel.cpp
 * \brief Implémentation d'un disque virtuel.
 * \author ?
 * \version 0.1
 * \date nov-dec 2020
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

	// Ajouter votre code ici !

	DisqueVirtuel::DisqueVirtuel(){
        m_blockDisque.resize(N_BLOCK_ON_DISK);

	}
	DisqueVirtuel::~DisqueVirtuel(){
	}
	    int DisqueVirtuel::bd_FormatDisk(){

	    m_blockDisque.clear(); // effacer le disque
	    m_blockDisque[FREE_BLOCK_BITMAP].m_type_donnees = S_IFBL;
        m_blockDisque[FREE_INODE_BITMAP].m_type_donnees = S_IFIL;

        for(int i = 24; i <=127 ; ++i){                       // initialise les blocs libres dans le bloc 2.
            m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i] = true;
        }

        for(int i = 0 ; i<=23; i++){   // Marque tous les blocs de 0 - 23 comme non-libre
            m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i] = false;
        }
	    for (int i = 4; i<=23; ++i){
	       // m_blockDisque[i].m_inode->st_ino =0;
            m_blockDisque[i].m_inode = new iNode(i, S_IFDIR, i, 28, 0);
	    }

	    for (int i = 1; i <=19; ++i) {    // Marque tous les inodes de 1 à 19 comme libre
	        m_blockDisque[FREE_INODE_BITMAP].m_bitmap[i]= true;
	    }

	    m_blockDisque[5].m_dirEntry[0] = new dirEntry(1, "/");
	    

//        for(int i = 4; i <=23 ; ++i) {
//            m_blockDisque[i].m_type_donnees = S_IFIN;
//        }
//        for(int i = 24; i <=127 ; ++i) {
//            m_blockDisque[i].m_type_donnees = S_IFDE;
//        }

	    return 0;
	}
	std::string DisqueVirtuel::bd_ls(const std::string& p_DirLocation){
	
	return p_DirLocation;
	}
	int DisqueVirtuel::bd_mkdir(const std::string& p_DirName){
	return 1;
	}
	int DisqueVirtuel::bd_create(const std::string& p_FileName)	
	{
	return 1;	
	}
	int DisqueVirtuel::bd_rm(const std::string& p_Filename){
	return 0;
	}

}//Fin du namespace
