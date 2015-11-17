import FWCore.ParameterSet.Config as cms

###OMTF ESProducer. Fills CondFormats from XML files.
###OMTF ConfFormats are loaded from sqlite file
omtfParamsSource = cms.ESSource(
    "EmptyESSource",
    recordName = cms.string('L1TMuonOverlapParamsRcd'),
    iovIsRunNotTime = cms.bool(True),
    firstValid = cms.vuint32(1)
)


omtfParams = cms.ESProducer(
    "L1TMTFOverlapParamsESProducer",
    configFromXML = cms.bool(False), #this is necessary as we contruct OMTFConfiguration inside ESProducer. This is a temporary solution.   
    patternsXMLFiles = cms.VPSet(
        cms.PSet(patternsXMLFile = cms.FileInPath("L1Trigger/L1TMuon/data/omtf_config/Patterns_ipt6_31.xml")),
    ),
    configXMLFile = cms.FileInPath("L1Trigger/L1TMuon/data/omtf_config/hwToLogicLayer.xml"),
)




