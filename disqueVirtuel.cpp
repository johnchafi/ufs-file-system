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
#include <sstream>
//vous pouvez inclure d'autres librairies si c'est nécessaire

namespace TP3
{
	// Ajouter votre code ici !

	DisqueVirtuel::DisqueVirtuel(){
        m_blockDisque.resize(N_BLOCK_ON_DISK);
	}
	DisqueVirtuel::~DisqueVirtuel(){
	}
        int DisqueVirtuel::findFreeBlock(){
            int freeBlock = 0;
            for(int i = 24; i<m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap.size(); i++)
            {
                if(m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i]==true )
                {
                    freeBlock = i;
                    return freeBlock;
                }
            }
            return freeBlock;
            }

    int DisqueVirtuel::findFreeInode(){
        int freeInode = 0;
        for(int i = 0; i<m_blockDisque[FREE_INODE_BITMAP].m_bitmap.size(); i++)
        {
            if(m_blockDisque[FREE_INODE_BITMAP].m_bitmap[i]==true )
            {
                freeInode = i;
                return freeInode;
            }
        }
        return freeInode;
        }


	    int DisqueVirtuel::bd_FormatDisk(){

	//    m_blockDisque.clear(); // effacer le disque
//	    if(m_blockDisque.size() != 128) {
//            return 0;
//	    }

        m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap.resize(N_BLOCK_ON_DISK);
        m_blockDisque[FREE_INODE_BITMAP].m_bitmap.resize(N_INODE_ON_DISK);
        m_blockDisque[FREE_BLOCK_BITMAP].m_type_donnees = S_IFBL;
        m_blockDisque[FREE_INODE_BITMAP].m_type_donnees = S_IFIL;

        for(int i = 0 ; i<=23; i++){   // Marque tous les blocs de 0 - 23 comme non-libre
            m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i] = false;
        }
        for(int i = 24 ; i< N_BLOCK_ON_DISK; i++){   // Marque tous les blocs de 24-128 comme non-libre
            m_blockDisque[FREE_BLOCK_BITMAP].m_bitmap[i] = true;
        }
	    for (int i = 4; i<=23; ++i){  // crée des inodes dans le les blocs 4-23
            m_blockDisque[i].m_inode = new iNode(i-4, S_IFDIR, 0, 0, 0);
	    }
	    for (int i = 1; i < N_INODE_ON_DISK; ++i) {  // Marque tous les inodes de 1 à 19 comme libre
	        m_blockDisque[FREE_INODE_BITMAP].m_bitmap[i]= true;
	    }
	    m_blockDisque[FREE_INODE_BITMAP].m_bitmap[0] = false;
	    int freeInode = findFreeInode();
	    if(freeInode == 0) return 0;
	    int freeBlock = findFreeBlock();
	    if(freeBlock == 0) return 0;
      //  m_blockDisque[freeInode+BASE_BLOCK_INODE].m_dirEntry[24] = new dirEntry(1, "/");
        dirEntry *racine = new dirEntry(ROOT_INODE , ".");
        dirEntry *parent = new dirEntry(ROOT_INODE , "..");
        m_blockDisque[freeBlock].m_dirEntry.push_back(racine);
        m_blockDisque[freeBlock].m_dirEntry.push_back(parent);
        m_blockDisque[freeInode+BASE_BLOCK_INODE].m_inode->st_block = freeBlock;
        m_blockDisque[freeInode+BASE_BLOCK_INODE].m_inode->st_size = 56;
        m_blockDisque[freeInode+BASE_BLOCK_INODE].m_inode->st_nlink = 2;
        m_blockDisque[FREE_INODE_BITMAP].m_bitmap[freeBlock] = false;
        std::cout<<"Saisie "<< m_blockDisque[freeInode+BASE_BLOCK_INODE].m_inode->st_ino<<std::endl;
	    std::cout<<"Saisie bloc "<< m_blockDisque[freeInode+BASE_BLOCK_INODE].m_inode->st_block<<std::endl;

	    return 1;
	}
	std::string DisqueVirtuel::bd_ls(const std::string& p_DirLocation) {
        std::ostringstream formate;
        if (p_DirLocation.substr(0, 1) == "/") {
            int blocDonne = m_blockDisque[5].m_inode->st_block;
            int inodeInfo = 0;
            for (auto it = m_blockDisque[blocDonne].m_dirEntry.begin();it != m_blockDisque[blocDonne].m_dirEntry.end(); ++it) {
                //            if(m_blockDisque[5].m_inode->st_mode == S_IFDIR)

                inodeInfo = (*it)->m_iNode;
                formate << "d" << "          " << (*it)->m_filename <<
                        "  Size: " << m_blockDisque[inodeInfo+BASE_BLOCK_INODE].m_inode->st_size <<
                        "  inode: " << m_blockDisque[inodeInfo+BASE_BLOCK_INODE].m_inode->st_ino <<
                        "  nlink: " << m_blockDisque[inodeInfo+BASE_BLOCK_INODE].m_inode->st_nlink << std::endl;
            }
        }
        return formate.str();
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

    int DisqueVirtuel::createEmptyRepertory(iNode * m_inode){
//        m_inode->st_nlink = 2;
//        m_inode->st_size = 0;
//        m_inode->st_block = 1;
//        m_inode->
//        return 0;

	}
    int DisqueVirtuel::findRepertoryExist(){
        return 0;
	}
	int isRepertory(iNode *inode){
        if (inode != NULL && inode->st_mode == S_IFDIR) return 1;
        else return 0;
	}
    int isFile(iNode *inode){
        if (inode != NULL && inode->st_mode == S_IFREG) return 1;
        else return 0;
    }


    int GetDirFromPath(const char *pPath, char *pDir) {
        strcpy(pDir,pPath);
        int len = strlen(pDir); // length, EXCLUDING null
        int index;

        // On va a reculons, de la fin au debut
        while (pDir[len]!='/') {
            len--;
            if (len <0) {
                // Il n'y avait pas de slash dans le pathname
                return 0;
            }
        }
        if (len==0) {
            // Le fichier se trouve dans le root!
            pDir[0] = '/';
            pDir[1] = 0;
        }
        else {
            // On remplace le slash par une fin de chaine de caractere
            pDir[len] = '\0';
        }
        return 1;
    }


    int GetFilenameFromPath(const char *pPath, char *pFilename) {
        // Pour extraire le nom de fichier d'un path complet
        char newPath[256];
        strcpy(newPath,pPath);
        char *pStrippedFilename = strrchr(newPath,'/');
        if (pStrippedFilename!=NULL) {
            ++pStrippedFilename; // On avance pour passer le slash
            if ((*pStrippedFilename) != '\0') {
                // On copie le nom de fichier trouve
                strcpy(pFilename, pStrippedFilename);
                return 1;
            }
        }
        return 0;
    }

}//Fin du namespace
