#include <config.h>
#include <cmtypes.h>
#include <smldata.h>
#include <smlproc.h>

cmlib::SmlHandlerBase::
SmlHandlerBase()
{
}
cmlib::SmlHandlerBase::
~SmlHandlerBase()
{
}


cmlib::SmlHandler::
SmlHandler(cmlib::SmlHandlerBase* p, cmlib::SmlNode* n)
  : parent_(p), node_(n)
{}
cmlib::SmlHandler::
~SmlHandler()
{}

cmlib::SmlHandlerBase* cmlib::SmlHandler::
open(cmlib::SmlNode* n, cmlib::v_proc_base*)
  throw(std::exception)
{
  return NULL;
}

void cmlib::SmlHandler::
proc_before(cmlib::v_proc_base*)
  throw(std::exception)
{
}

void cmlib::SmlHandler::
proc_after(cmlib::v_proc_base*)
  throw(std::exception)
{
}


struct SmlItem_t
{
  SmlItem_t(cmlib::SmlHandlerBase *hh,
            cmlib::SmlNode* nn,
            bool cl=true)
    : h(hh), n(nn), pos(0), clean(cl)
  {}

  cmlib::SmlHandlerBase *h;
  cmlib::SmlNode* n;
  unsigned int pos;
  bool clean;
};


// ���� � ������������. ����� ��� ����������� �����������
// ��������� ��������-������������ ��� ������� ����������.
class SmlStack_t : public std::stack<SmlItem_t>
{
public:
  typedef std::stack<SmlItem_t> inherited;
public:
  void pop()
  {
    if ( ! empty() ) {
      if ( top().clean )
        delete top().h;
      top().h = NULL;
      inherited::pop();
    }
  }

  virtual ~SmlStack_t()
  {
    while ( ! empty() ) {
      if ( top().clean )
        delete top().h;
      top().h = NULL;
      pop();
    }
  }
};


void cmlib::
SmlProcessTree(cmlib::SmlHandlerBase* handler,
               cmlib::SmlNode* root,
               cmlib::v_proc_base* data)
  throw(std::exception)
{
  static const char* Method = "cmlib::SmlProcessTree()";
  SmlStack_t stack;

  cmlib::SmlHandlerBase *gh;
  cmlib::SmlNode *gn;
  unsigned int gpos;

  if ( handler==NULL || root==NULL )
    cmlib_throw(Method, "invalid arguments");

  stack.push(SmlItem_t(handler, root, false));
  gh = handler; gn = root;
  gh -> proc_before(data);

  while ( ! stack.empty() )
    {
      gh = stack.top().h;  gn = stack.top().n;
      gpos = stack.top().pos;
      if ( gh==NULL || gn==NULL )
        {
          stack.pop();
          continue;
        }
      if ( gpos >= gn->nodeCount() )
        {
          gh -> proc_after(data);
          stack.pop();
          continue;
        }

      stack.top().pos ++;
      stack.push(SmlItem_t(NULL,NULL,true));
      gn = gn->getChildNode(gpos);
      gh = gn ? gh->open(gn, data) : NULL;
      if ( gh ) {
        stack.top().h = gh;
        stack.top().n = gn;
        gh -> proc_before(data);
      } else
        stack.pop();
    }
}
