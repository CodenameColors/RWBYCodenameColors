// Authour: Antonio Morales.

#pragma once

#include "Runtime/XmlParser/Public/XmlParser.h"

/**
 * 
 */

struct FComboDataStruct {

	UPROPERTY()
		FString Animation;
	UPROPERTY()
		FString Particle;
	UPROPERTY()
		FString SoundFX;
	UPROPERTY()
		FString Mesh;
	UPROPERTY()
		FString Dialogue;
	//more later;

};


enum EAttackTypes {

	None,
	Light,
	Heavy,
	Light_Dash,
	Heavy_Dash

};


class RWBY_CODENAMECOLORS_API RubyRoseComboNode
{
public:
	RubyRoseComboNode();
	~RubyRoseComboNode();

	//Node Properties
	FComboDataStruct Data;

	RubyRoseComboNode* Light;
	RubyRoseComboNode* Heavy;
	RubyRoseComboNode* LightDash;
	RubyRoseComboNode* HeavyDash;

	EAttackTypes NodeType;

	RubyRoseComboNode(FString Animation, EAttackTypes Type);
	void FilleComboTree(RubyRoseComboNode* Root, TArray<FString> Animation, TArray<EAttackTypes> Type);

	bool HasChildren();

	bool DoesExist(EAttackTypes Type);

	void CreateTree();

	void CreateTreeRecursively(RubyRoseComboNode* CurrerntRoot, const FXmlNode* CurrentRootNode, int CurrentMainTreeBranch);

	FString GetInnnerXML(FString DesiredTag, const FXmlNode* CurrentRootNode, FString BaseToContain);

	FString GetAssetName(FString DesiredAssetType, FString AllInnerContent);


};
