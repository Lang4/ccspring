#include "ccxml.h"
#include "tinyxml2.h"
#include <vector>
#include <map>
#include "ccspring.h"
class Tiny2Node : public INode{
public:
	Tiny2Node(tinyxml2::XMLElement *node) :_node(node)
	{

	}
	Tiny2Node(){}
	/**
	* 平行节点的下一个节点
	*/
	virtual INode* nextNode(const char *name)
	{
		return newNext(_node->NextSiblingElement(name));
	}
	/**
	* 获取节点属性
	*/
	virtual std::string getStr(const char *name)
	{
		return (_node->Attribute(name) ? _node->Attribute(name):"");
	}

	/**
	* 获取子节点
	*/
	virtual INode* childNode(const char *name)
	{
		return newName(name,_node->FirstChildElement(name));
	}
private:
	tinyxml2::XMLElement *_node;
	std::string next;
	std::map<std::string,std::string> childs;
	friend class Tiny2Doc;

	Tiny2Node *newNext(tinyxml2::XMLElement *ele)
	{
		if (!ele) return 0;
		next.resize(sizeof(Tiny2Node) + sizeof(int));
		Tiny2Node * next1 = new (&next[0]) Tiny2Node(ele);
		return next1;
	}
	Tiny2Node *newName(const char *name,tinyxml2::XMLElement *ele)
	{
		if (!ele) return 0;
		std::string & child = childs[name];
		child.resize(sizeof(Tiny2Node) + sizeof(int));
		Tiny2Node * node = new (&child[0]) Tiny2Node(ele);
		return node;
	}
};

class Tiny2Doc : public IXML, public Creator<Tiny2Doc>{
public:
	bool initFrom(const char *filename)
	{
		if (doc.LoadFile(filename)) return false;
		return true;
	}

	INode*  firstNode(const char *name)
	{
		tinyxml2::XMLElement * ele = doc.RootElement();
		root._node = ele->FirstChildElement(name);
		if (root._node == 0) return 0;
		return &root;
	}
private:
	tinyxml2::XMLDocument doc;
	Tiny2Node root;
};

void XML::imp()
{
	setImp<Tiny2Doc>();
}

CREATOR("tinyxml2",Tiny2Doc);