// -------------------------------------------------------------------------
//    @FileName         :    NFCSkillModule.cpp
//    @Author               :    LvSheng.Huang
//    @Date                 :    2013-06-11
//    @Module               :    NFCSkillModule
//    @Desc                 :
// -------------------------------------------------------------------------

#include "NFCSkillModule.h"

bool NFCSkillModule::Init()
{
    mstrSkillTableName = "SkillList";
    return true;
}


bool NFCSkillModule::Shut()
{
    return true;
}

bool NFCSkillModule::Execute( const float fLasFrametime, const float fStartedTime )
{
    //位置呢
    return true;
}

bool NFCSkillModule::AfterInit()
{
    m_pEventProcessModule = dynamic_cast<NFIEventProcessModule*>( pPluginManager->FindModule( "NFCEventProcessModule" ) );
    m_pKernelModule = dynamic_cast<NFIKernelModule*>( pPluginManager->FindModule( "NFCKernelModule" ) );
    m_pSkillConsumeManagerModule = dynamic_cast<NFISkillConsumeManagerModule*>( pPluginManager->FindModule( "NFCSkillConsumeManagerModule" ) );
    m_pElementInfoModule = dynamic_cast<NFIElementInfoModule*>( pPluginManager->FindModule( "NFCElementInfoModule" ) );
    m_pLogModule = dynamic_cast<NFILogModule*>( pPluginManager->FindModule( "NFCLogModule" ) );
    m_pPropertyModule = dynamic_cast<NFIPropertyModule*>( pPluginManager->FindModule( "NFCPropertyModule" ) );
    m_pSceneProcessModule = dynamic_cast<NFISceneProcessModule*>( pPluginManager->FindModule( "NFCSceneProcessModule" ) );

    

    assert( NULL != m_pEventProcessModule );
    assert( NULL != m_pKernelModule );
    assert( NULL != m_pSkillConsumeManagerModule );
    assert( NULL != m_pElementInfoModule );
    assert( NULL != m_pLogModule );
    assert( NULL != m_pPropertyModule );
    assert( NULL != m_pSceneProcessModule );

    m_pEventProcessModule->AddEventCallBack( NFIDENTID(), NFED_ON_CLIENT_REQUIRE_USE_SKILL, this, &NFCSkillModule::OnRequireUseSkillEvent );
    m_pEventProcessModule->AddEventCallBack( NFIDENTID(), NFED_ON_CLIENT_REQUIRE_USE_SKILL_POS, this, &NFCSkillModule::OnRequireUseSkillPosEvent );

    m_pEventProcessModule->AddClassCallBack( "Player", this, &NFCSkillModule::OnClassObjectEvent );
    m_pEventProcessModule->AddClassCallBack( "NPC", this, &NFCSkillModule::OnClassObjectEvent );
    m_pEventProcessModule->AddClassCallBack( "Pet", this, &NFCSkillModule::OnClassObjectEvent );


    return true;
}

int NFCSkillModule::OnClassObjectEvent( const NFIDENTID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& var )
{
    if ( CLASS_OBJECT_EVENT::COE_DESTROY == eClassEvent )
    {

    }
    else if ( CLASS_OBJECT_EVENT::COE_CREATE_NODATA == eClassEvent )
    {

    }
    else if ( CLASS_OBJECT_EVENT::COE_CREATE_HASDATA == eClassEvent )
    {
    }
    else if ( CLASS_OBJECT_EVENT::COE_CREATE_FINISH == eClassEvent )
    {

    }

    return 0;
}

int NFCSkillModule::OnUseSkill(const NFIDENTID& self, const NFIDataList& var)
{
    if ( var.GetCount() != 2)
    {
        return 1;
    }

    const std::string& strSkillID = var.String( 0 );
    const NFIDENTID& nTargetID = var.Object(1);

    NF_SHARE_PTR<NFIObject> pObejct = m_pKernelModule->GetObject(self);
    if ( pObejct == NULL )
    {
        return 1;
    }

    //NF_SHARE_PTR<NFIRecord> pRecordSkill = pObejct->GetRecordManager()->GetElement( mstrSkillTableName );
    //if ( pRecordSkill == NULL )
    //{
    //    return 1;
    //}

    //NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = m_pElementInfoModule->GetPropertyManager( var.String( 2 ) );
    //if ( pPropertyManager == NULL )
    //{
    //    return 1;
    //}

    //NF_SHARE_PTR<NFIProperty> pItemTypeProperty = pPropertyManager->GetElement( "SkillType" );
    //if ( pItemTypeProperty == NULL )
    //{
    //    return 1;
    //}

    //配置表中真的有这么个技能类别
    //EGameSkillType eItemType = ( EGameSkillType )pItemTypeProperty->GetInt();

    if (!m_pElementInfoModule->ExistElement(strSkillID))
    {
        return 1;
    }

    //NFISkillConsumeProcessModule* pConsumeProcessModule = m_pSkillConsumeManagerModule->GetConsumeModule( EGameSkillType::EGST_JOBSKILL_BRIEF );
    //if ( pConsumeProcessModule == NULL )
    //{
    //    return 1;
    //}

    //NFCDataList damageValueList;
    //NFCDataList damageResultList;
    //int nResult = pConsumeProcessModule->ConsumeProcess( var.Object( 0 ), var.String( 2 ), valueOther, damageValueList, damageResultList );
    //for (int i = 0; i < valueOther.GetCount(); i++)
    //{
    //    m_pKernelModule->SetPropertyInt(valueOther.Object(i), "HP", 0);
    //    damageValueList.AddInt(0);
    //    damageResultList.AddInt(0);
    //}

    int nCurHP = m_pKernelModule->GetPropertyInt(nTargetID, "HP");
    if (nCurHP <= 0)
    {
        return 1;
    }

    m_pKernelModule->SetPropertyObject(nTargetID, "LastAttacker", self);
    m_pKernelModule->SetPropertyInt(nTargetID, "HP", (nCurHP - 10) >= 0 ? (nCurHP - 10) : 0); // 暂时扣10点血
    

    ////结果事件--无论失败或者是成功，都会发下去--当然使用结果只对使用者下发--成果的结果，还得对被施放的人发
    //if ( damageValueList.GetCount() == damageResultList.GetCount()
    //    && damageValueList.GetCount() == valueOther.GetCount() )
    //{
    //    NFCDataList valueResult;
    //    valueResult.AddString( var.String( 2 ).c_str() );
    //    valueResult.AddInt( valueOther.GetCount() );
    //    valueResult.Append( valueOther, 0, valueOther.GetCount() ); //伤害对象
    //    valueResult.Append( damageValueList, 0, damageValueList.GetCount() ); //伤害值
    //    valueResult.Append( damageResultList, 0, damageResultList.GetCount() ); //击打效果

    //    //现在不需要反馈，杀了就杀了
    //    //m_pEventProcessModule->DoEvent( pObejct->Self(), NFED_ON_CLIENT_USE_SKILL_RESULT, valueResult );
    //}

    return 0;
}

int NFCSkillModule::OnRequireUseSkillEvent( const NFIDENTID& self, const int nEventID, const NFIDataList& var )
{
    if ( var.GetCount() < 3)
    {
        return 1;
    }

    //EGameErrorCode errorCode = EGameErrorCode::EGEC_INVALID_SKILL;
    NF_SHARE_PTR<NFIObject> pObejct = m_pKernelModule->GetObject( var.Object( 0 ) );
    if ( pObejct == NULL )
    {
        return 1;
    }

    NF_SHARE_PTR<NFIRecord> pRecordSkill = pObejct->GetRecordManager()->GetElement( mstrSkillTableName );
    if ( pRecordSkill == NULL )
    {
        return 1;
    }

    NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = m_pElementInfoModule->GetPropertyManager( var.String( 2 ) );
    if ( pPropertyManager == NULL )
    {
        return 1;
    }

    NF_SHARE_PTR<NFIProperty> pItemTypeProperty = pPropertyManager->GetElement( "SkillType" );
    if ( pItemTypeProperty == NULL )
    {
        return 1;
    }

    //配置表中真的有这么个技能类别
    EGameSkillType eItemType = ( EGameSkillType )pItemTypeProperty->GetInt();
    NFISkillConsumeProcessModule* pConsumeProcessModule = m_pSkillConsumeManagerModule->GetConsumeModule( EGameSkillType::EGST_JOBSKILL_BRIEF );
    if ( pConsumeProcessModule == NULL )
    {
        return 1;
    }

    NFCDataList valueOther;
    valueOther.Append( var, 3, var.GetCount() - 3 ); // 被攻击玩家数量 3表示从第几个参数开始是被攻击玩家
    //     if ( pConsumeProcessModule->ConsumeLegal( var.ObjectVal( 0 ), var.StringVal( 2 ), valueOther ) != 0 )
    //     {
    //         return 1;
    //     }
    // 
    //     if ( pConsumeProcessModule->ConsumeSelf( var.ObjectVal( 0 ), var.StringVal( 2 ) ) != 0 )
    //     {
    //         return 1;
    //     }
    // 
    NFCDataList damageValueList;
    NFCDataList damageResultList;
    int nResult = pConsumeProcessModule->ConsumeProcess( var.Object( 0 ), var.String( 2 ), valueOther, damageValueList, damageResultList );
    for (int i = 0; i < valueOther.GetCount(); i++)
    {
        m_pKernelModule->SetPropertyInt(valueOther.Object(i), "HP", 0);
        damageValueList.AddInt(0);
        damageResultList.AddInt(0);
    }

    //结果事件--无论失败或者是成功，都会发下去--当然使用结果只对使用者下发--成果的结果，还得对被施放的人发
    if ( damageValueList.GetCount() == damageResultList.GetCount()
        && damageValueList.GetCount() == valueOther.GetCount() )
    {
        NFCDataList valueResult;
        valueResult.AddString( var.String( 2 ).c_str() );
        valueResult.AddInt( valueOther.GetCount() );
        valueResult.Append( valueOther, 0, valueOther.GetCount() ); //伤害对象
        valueResult.Append( damageValueList, 0, damageValueList.GetCount() ); //伤害值
        valueResult.Append( damageResultList, 0, damageResultList.GetCount() ); //击打效果

        //现在不需要反馈，杀了就杀了
        //m_pEventProcessModule->DoEvent( pObejct->Self(), NFED_ON_CLIENT_USE_SKILL_RESULT, valueResult );
    }

    return 0;
}

int NFCSkillModule::OnRequireUseSkillPosEvent( const NFIDENTID& self, const int nEventID, const NFIDataList& var )
{
    if ( var.GetCount() < 4 ||
        !var.TypeEx(TDATA_OBJECT, TDATA_FLOAT, TDATA_FLOAT, TDATA_FLOAT, TDATA_OBJECT))
    {
        return 1;
    }

    int nSkillID = var.Int( 0 );
    float fX = var.Float( 1 );
    float fY = var.Float( 2 );
    float fZ = var.Float( 3 );
    //群伤，就只计算第一个人的闪避

    //结果事件
    //  NFCDataList valueResult;
    //  valueResult.AddInt(nSkillID);
    //  valueResult.AddInt(nResult);
    //  valueResult.Append(var, 4, var.GetCount());
    //
    //  m_pEventProcessModule->DoEvent(self, NFED_ON_CLIENT_USE_SKILL_POS_RESULT, valueResult);

    return 0;
}

int NFCSkillModule::AddSkill( const NFIDENTID& self, const std::string& strSkillName )
{
    if(m_pElementInfoModule->ExistElement(strSkillName))
    {
        if ( ExistSkill( self, strSkillName ) < 0 )
        {
            NF_SHARE_PTR<NFIRecord> pRecord =  m_pKernelModule->FindRecord( self, mstrSkillTableName );
            if ( pRecord )
            {
                return pRecord->AddRow( -1,  NFCDataList() << strSkillName.c_str());
                //                 if ( nRow >= 0 )
                //                 {
                //                     return pRecord->SetString( nRow, EGameSkillStoreType::EGSST_TYPE_SKILL_CONFIGID, strSkillName.c_str() );
                //                 }
            }
        }
    }

    m_pLogModule->LogElement(NFILogModule::NLL_ERROR_NORMAL, NFIDENTID(), strSkillName, "There is no element", __FUNCTION__, __LINE__);

    return 0;
}

int NFCSkillModule::ExistSkill( const NFIDENTID& self, const std::string& strSkillName )
{
    NF_SHARE_PTR<NFIRecord> pRecord =  m_pKernelModule->FindRecord( self, mstrSkillTableName );
    if ( pRecord )
    {
        NFCDataList varResult;
        pRecord->FindString( EGameSkillStoreType::EGSST_TYPE_SKILL_CONFIGID, strSkillName.c_str(), varResult );
        if ( varResult.GetCount() == 1 )
        {
            return varResult.Int( 0 );
        }
    }

    return -1;
}

int NFCSkillModule::SetSkillLevel( const NFIDENTID& self, const std::string& strSkillName, const int nLevel )
{
    //     int nFindRow = ExistSkill( self, strSkillName );
    //     if ( nFindRow >= 0 )
    //     {
    //         m_pKernelModule->SetRecordInt( self, mstrSkillTableName, nFindRow, EGameSkillStoreType::EGSST_TYPE_SKILL_LEVEL, nLevel );
    //     }

    return 0;
}

int NFCSkillModule::SetSkillGem( const NFIDENTID& self, const std::string& strSkillName, const std::string& strGemName )
{
    int nFindRow = ExistSkill( self, strSkillName );
    if ( nFindRow >= 0 )
    {
        //m_pKernelModule->SetRecordString( self, mstrSkillTableName, nFindRow, EGameSkillStoreType::EGSST_TYPE_SKILL_GEM_CONFIGID, strGemName.c_str() );
    }

    return 0;
}

int NFCSkillModule::SetSkillGemLevel( const NFIDENTID& self, const std::string& strSkillName, const int nLevel )
{
    int nFindRow = ExistSkill( self, strSkillName );
    if ( nFindRow >= 0 )
    {
        //m_pKernelModule->SetRecordInt( self, mstrSkillTableName, nFindRow, EGameSkillStoreType::EGSST_TYPE_SKILL_GEM_LEVEL, nLevel );
    }

    return 0;
}

int NFCSkillModule::GetSkillLevel( const NFIDENTID& self, const std::string& strSkillName )
{
    //     int nFindRow = ExistSkill( self, strSkillName );
    //     if ( nFindRow >= 0 )
    //     {
    //         m_pKernelModule->QueryRecordInt( self, mstrSkillTableName, nFindRow, EGameSkillStoreType::EGSST_TYPE_SKILL_LEVEL );
    //     }

    return 0;
}

std::string NFCSkillModule::GetSkillGem( const NFIDENTID& self, const std::string& strSkillName )
{
    int nFindRow = ExistSkill( self, strSkillName );
    if ( nFindRow >= 0 )
    {
        //return m_pKernelModule->QueryRecordString( self, mstrSkillTableName, nFindRow, EGameSkillStoreType::EGSST_TYPE_SKILL_GEM_CONFIGID );
    }

    return NULL_STR;
}

int NFCSkillModule::GetSkillGemLevel( const NFIDENTID& self, const std::string& strSkillName )
{
    int nFindRow = ExistSkill( self, strSkillName );
    if ( nFindRow >= 0 )
    {
        //return m_pKernelModule->QueryRecordInt( self, mstrSkillTableName, nFindRow, EGameSkillStoreType::EGSST_TYPE_SKILL_GEM_LEVEL );
    }

    return 0;
}

int NFCSkillModule::AddNewerSkill( const NFIDENTID& self )
{
    //全部技能扫一遍
    //角色第一次上线的话，添加技能
    //初始化技能列表

    return 0;
}
