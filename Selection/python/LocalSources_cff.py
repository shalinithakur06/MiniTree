import FWCore.ParameterSet.Config as cms

import os,commands,time
def fillFrom(dirname):
    filelist = cms.untracked.vstring()
    for subdir, dirs, files in os.walk(dirname):
        for file in files:
            if(file.find('.root') == -1 ) : continue
            if(file.find('Monitor') != -1 ) : continue
            filelist.append('file:///'+dirname+'/'+file)
    return filelist

def mergeFileLists( fileListArray ):
    filelist = cms.untracked.vstring()
    for l in fileListArray :
        for f in l :
            filelist.append(f)
    return filelist

#data
dilcands = fillFrom('/lustre/lip.pt/data/cmslocal/pedros/DileptonCandidates2010')

#mc
ttbaraod=fillFrom('/lustre/lip.pt/data/cmslocal/pedros/TT1Jets-alpgen-aod')
ttbarRelVal384 = fillFrom('/lustre/lip.pt/data/cmslocal/pedros/RelVal_3_8_4/')
dyjetstoll = fillFrom('/lustre/lip.pt/data/cmslocal/pedros/Fall10/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/')
chhiggs = fillFrom('/lustre/lip.pt/data/cmslocal/nalmeida/mc/test/CMSSW_38X/HHtotaunu120/')
w2munu = fillFrom('/lustre/lip.pt/data/cmslocal/samples/CMSSW_3_8_6/w2MuNu')

#NICE WAY TO PRINT STRINGS
def toPrint(string, value):
    length = (len(string)+len(str(value))+2)
    line = "-"*length
    print ""
    print "* "+ line +                    " *"
    print "| "+ " "*length +              " |"
    print "| "+ string+ ": "+ str(value)+ " |"
    print "| "+ " "*length +              " |"
    print "* "+ line +                    " *"

#/////////////////////////////////////////////////////#





