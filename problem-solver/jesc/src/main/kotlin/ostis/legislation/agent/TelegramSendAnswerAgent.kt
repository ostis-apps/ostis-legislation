package ostis.legislation.agent

import net.ostis.jesc.agent.ScAgent
import net.ostis.jesc.client.ScClient
import net.ostis.jesc.client.model.element.ScReference
import net.ostis.jesc.client.model.response.ScEvent
import net.ostis.jesc.client.model.type.ScType
import ostis.legislation.extension.getRoleRelationTarget
import ostis.legislation.thirdparty.telegram.Telegram

class TelegramSendAnswerAgent(
    private val telegram: Telegram,
    event: Long,
    client: ScClient
): ScAgent(setOf(event), client) {

    private val rrelTelegramChatId =
        context.resolveBySystemIdentifier("rrel_telegram_chat_id", ScType.NODE_CONST_ROLE)

    override fun onTrigger(event: ScEvent) {
        val answerRelation = event.payload[2]

        val addrs = context.api.searchByTemplate()
            .references(
                ScReference.type(ScType.VAR),
                ScReference.addr(answerRelation),
                ScReference.type(ScType.LINK_VAR)
            )
            .execute().payload.addrs[0]

        val telegramId = context.getRoleRelationTarget(addrs[0], rrelTelegramChatId)
            .let { context.getLinkContent(it).asLong() }
        val answerText = context.getLinkContent(addrs[2]).asText()

        telegram.sendMessage(telegramId, answerText)
    }

}
