import net.ostis.jesc.agent.ScAgentRegistry
import net.ostis.jesc.api.ScApi
import net.ostis.jesc.client.ScClient
import net.ostis.jesc.client.model.element.ScEventType
import net.ostis.jesc.client.model.type.ScType
import net.ostis.jesc.context.ScContext
import net.ostis.jesc.context.ScContextCommon
import ostis.legislation.WitAISCQueryCreator
import ostis.legislation.agent.TelegramSendAnswerAgent
import ostis.legislation.agent.WhatIsAgent
import ostis.legislation.thirdparty.telegram.Telegram
import ostis.legislation.thirdparty.witai.WitAI

fun prepareContext(): ScContext {
    val scClient = ScClient("localhost", 8090)
    val scApi = ScApi(scClient)
    return ScContextCommon(scApi)
}

fun prepareTelegram(context: ScContext, witAI: WitAI) = Telegram(TELEGRAM_TOKEN).apply {

    newUpdateHandler {
        if (it.hasMessage()) {
            val queryCreator = WitAISCQueryCreator(context, witAI, it.message.chatId)
            queryCreator.createQuery(it.message.text)
        }
    }

}

fun main() {
    val witAI = WitAI(WIT_AI_TOKEN)
    val context = prepareContext()
    val agentRegistry = ScAgentRegistry(context.api.client)

    val question = context.resolveBySystemIdentifier("question_natural_lang", ScType.NODE_CONST_CLASS)
    val answer = context.resolveBySystemIdentifier("rrel_answer_natural_lang", ScType.NODE_CONST_ROLE)

    val newQuestionEvent = context.createEvent(ScEventType.ADD_OUTGOING_EDGE, question)
    val resultReadyEvent = context.createEvent(ScEventType.ADD_OUTGOING_EDGE, answer)

    val telegram = prepareTelegram(context, witAI)

    agentRegistry.registerAgent(WhatIsAgent(newQuestionEvent, context.api.client))
    agentRegistry.registerAgent(TelegramSendAnswerAgent(telegram, resultReadyEvent, context.api.client))

    Runtime.getRuntime().addShutdownHook(Thread {
        println("Shutting down JESC agents.")
    })
}

const val TELEGRAM_TOKEN = "6130969742:AAFRmCjEQF37J4SIQQnPMmG_k--YdlGNR-I"
const val WIT_AI_TOKEN = "OCBKFILCPZAW4KVB4PWIMZXKVAFDRBUN"
