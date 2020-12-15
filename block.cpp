/**
 * \file block.cpp
 * \brief Implémentation d'un bloc.
 * \author ?
 * \version 0.1
 * \date nov-dec 2020
 *
 *  Travail pratique numéro 3
 *
 */

#include "block.h"
//vous pouvez inclure d'autres librairies si c'est nécessaire

namespace TP3
{
	// Ajouter votre code ici !
	Block::Block(){
	}
		
	Block::Block(size_t td){
	    m_type_donnees = td;
	}
	Block::~Block(){
	    m_bitmap.clear();
	    for(size_t i = 0; i<m_dirEntry.size(); i++)
        {
	        delete m_dirEntry[i];
        }
	}
}

//}//Fin du namespace




