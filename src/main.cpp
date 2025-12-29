#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/DeclCXX.h"

using namespace clang;

class FindNamedClassVisitor : public RecursiveASTVisitor<FindNamedClassVisitor> 
{

    ASTContext *Context;

public:
    explicit FindNamedClassVisitor(ASTContext *Context)
        : Context(Context) 
    {}

    /*
     
    @purpose:       Visits ASTNode and checks to see if the name is equivalent.
    
    @param:         [in]                CXXRecordDecl       Generically Represents a 
                                                            C++ struct/class/union.

                                        return
    
    @code:          bool                                    Whether the node was
                                                            found or not. 
    
    
    @notes:         This function ...
                    
    */

    // bool VisitCXXRecordDecl(CXXMethodDecl* Declaration)  // WHY DOESN'T THIS WORK?
    
    bool VisitCXXFunctionDecl(FunctionDecl* Declaration) 
    {
        llvm::outs() << Declaration->getQualifiedNameAsString() << "\n"; 
        if (Declaration->getQualifiedNameAsString() == "" && false) 
        {
            /* 
                * Error:    Trying to invoke Declaration::getLocStart() when that doesn't exist.
                *           We are trying to find a source location and it's associated source 
                *           manager. A source location represents a specific location in the 
                *           source code.
                *
                * Fix:      Convert a CXXRecordDecl into a SourceLocation. Assuming the CXXRecordDecl
                *           is an AST node somewhere within the source code we should be able to convert 
                *           it. 
            */

            FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getLocation());
            FullSourceLoc StartFullLocation = Context->getFullLoc(Declaration->getBeginLoc());
            if (FullLocation.isValid())
                llvm::outs() << "Found declaration at "
                            << FullLocation.getSpellingLineNumber() << ":"
                        << FullLocation.getSpellingColumnNumber() << "\n";
        }
        return true;
    }

    bool VisitDecl(Decl* D) {
    auto k = D->getDeclKindName();
    auto r = D->getSourceRange();
    auto b = r.getBegin();
    auto e = r.getEnd();
    auto& srcMgr = Context->getSourceManager();
    if (srcMgr.isInMainFile(b)) {
        auto fname = srcMgr.getFilename(b);
        auto bOff = srcMgr.getFileOffset(b);
        auto eOff = srcMgr.getFileOffset(e);
        llvm::outs() << k << "Decl ";
        llvm::outs() << "<" << fname << ", " << bOff << ", " << eOff << "> ";
        if (D->getKind() == Decl::Kind::Function) {
        auto fnDecl = reinterpret_cast<FunctionDecl*>(D);
        llvm::outs() << fnDecl->getNameAsString() << " ";
        llvm::outs() << "'" << fnDecl->getType().getAsString() << "' ";
        } else if (D->getKind() == Decl::Kind::ParmVar) {
        auto pvDecl = reinterpret_cast<ParmVarDecl*>(D);
        llvm::outs() << pvDecl->getNameAsString() << " ";
        llvm::outs() << "'" << pvDecl->getType().getAsString() << "' ";
        }
        llvm::outs() << "\n";
    }
    return true;
    }

    bool VisitCXXRecordDecl(CXXRecordDecl* Declaration) 
    {
        llvm::outs() << Declaration->getQualifiedNameAsString() << "\n"; 
        if (Declaration->getQualifiedNameAsString() == "" && false) 
        {
            /* 
                * Error:    Trying to invoke Declaration::getLocStart() when that doesn't exist.
                *           We are trying to find a source location and it's associated source 
                *           manager. A source location represents a specific location in the 
                *           source code.
                *
                * Fix:      Convert a CXXRecordDecl into a SourceLocation. Assuming the CXXRecordDecl
                *           is an AST node somewhere within the source code we should be able to convert 
                *           it. 
            */

            FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getLocation());
            FullSourceLoc StartFullLocation = Context->getFullLoc(Declaration->getBeginLoc());
            if (FullLocation.isValid())
                llvm::outs() << "Found declaration at "
                            << FullLocation.getSpellingLineNumber() << ":"
                        << FullLocation.getSpellingColumnNumber() << "\n";
        }
        return true;
    }
};

/* Allows for us to read the AST */

class FindNamedClassConsumer : public clang::ASTConsumer 
{

public:
    explicit FindNamedClassConsumer(ASTContext *Context)
        : Visitor(Context) 
    {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
  FindNamedClassVisitor Visitor;
};


/*
    @purpose:       Derives from ASTFrontendAction which is the interface for clients to 
                    hook into the compilation process and write custom logic to be performed 
                    on the internal representation of a source file. 

                                       return

    @notes:         This function delegates platform logic and does not itself implement
                    partition logic directly.
*/

class FindNamedClassAction : public clang::ASTFrontendAction 
{

public:
    
    /*
     
    @purpose:       Creates reference to an ASTConsumer
    
    
    @param:         [in]                CompilerInstance    Helper class for managing 
                                                            a single instance of the
                                                            Clang compiler.

    @param:         [in]                StringRef           Represents a constant reference
                                                            to a string.
    
                                        return
    
    @code:          std::unique_ptr<clang::ASTConsumer>     Uniquely owning reference
                                                            to the ASTConsumer.
    
    
    @notes:         This function ...
                    
    */
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer( clang::CompilerInstance &Compiler, llvm::StringRef InFile)
    {
        return std::unique_ptr<clang::ASTConsumer>(new FindNamedClassConsumer(&Compiler.getASTContext()));
    }
};

int main(int argc, char **argv) {
    if (argc > 1) 
    {
        std::unique_ptr<FrontendAction> NamedClassActionJob = std::make_unique<FindNamedClassAction>();

        /* 
            * Error:    Tried pass an already existing unique pointer by value and invoking the copy constructor which has been deleted.
            *           Make call to move constructor instead through use of std::move(). 
        */
        clang::tooling::runToolOnCode(std::move(NamedClassActionJob) , argv[1]); 
    }
    return EXIT_SUCCESS;
}
